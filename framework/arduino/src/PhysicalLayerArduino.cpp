/*	
    Half-duplex 1-wire connections between multiple Arduino and RPi
    Copyright (C) 2014  JoãSalavisa (joao.salavisa@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PhysicalLayer.h"
#include "PrintUtil.h"
#include <SoftwareSerial.h>
#include <Arduino.h>

// Constructor
PhysicalLayer::PhysicalLayer(uint8_t tx_pin, uint8_t rx_pin, char * device, uint32_t baud) {
    _tx_pin = tx_pin;
    _rx_pin = rx_pin;

    SoftwareSerial * line = new SoftwareSerial(_rx_pin, _tx_pin);
    set_tx_pin_to_input();
    (*line).begin(baud);

    _media_access = (void *)line;

}

// write a byte using the tx line if available. If the line is busy a ELBSY
// is returned, otherwise the amount of bytes written are returned
// if block is true we'll wait until the line is free to transmit
uint32_t PhysicalLayer::write(uint8_t byte, bool block) {
    bool line_available = probe_tx_line();
    if (!block && !line_available) {
        return ELBSY;
    } else if (block && !line_available) {
        //we wait until the line is free
        while(probe_tx_line());
    }

    // the tx pin is only set to output during the transmission itself
    noInterrupts();
    set_tx_pin_to_output();
    uint32_t result = 0;

    result += (*static_cast<SoftwareSerial *>(_media_access)).write(byte);
    delay(DEFAULT_WAIT_TX_SND);
    set_tx_pin_to_input();
    interrupts();
    return result;
}

// write a buffer of using the tx line if available. If the line is busy a ELBSY
// is returned, otherwise the amount of bytes written are returned
// if block is true we'll wait until the line is free to transmit
uint32_t PhysicalLayer::write(uint8_t * buffer, uint32_t size, bool block) {
    bool line_available = probe_tx_line();

    if (!block && !line_available) {
        return ELBSY;
    } else if (block && !line_available) {
        // wait until the line is free
        while(probe_tx_line());
    }
    PrintUtl.prints("Line not busy, gonna write %d\n", size);
    // the tx pin is only set to output during the transmission itself
    noInterrupts();
    set_tx_pin_to_output();
    uint32_t result = 0;
    for(int i = 0; i < size; i++) {
        result += (*static_cast<SoftwareSerial *>(_media_access)).write(buffer[i]);
    }
    delay(DEFAULT_WAIT_TX_SND);
    set_tx_pin_to_input();
    interrupts();

    return result;
}

// reads a byte of the rx line and returns it
uint32_t PhysicalLayer::read(uint8_t * byte) {
    if (available() < 1) {
        return ENAVL;
    }
    *byte = (*static_cast<SoftwareSerial *>(_media_access)).read();
    return 1;
}


// reads <code>size<code> bytes if available on the rx line
uint32_t PhysicalLayer::read(uint8_t * buffer, uint32_t size) {
    if (available() < size) {
        return ENAVL;
    }

    uint32_t result = 0;
    for(int i = 0; i < size; i++) {
        buffer[i] = (*static_cast<SoftwareSerial *>(_media_access)).read();
        result++;
    }
    return result;
}

// returns the amount number of bytes available to read in the rx line
uint32_t PhysicalLayer::available() {
    return (*static_cast<SoftwareSerial *>(_media_access)).available();
}

//
// Private methods
//
// Very naive implementation of the probe line. If we want to have a half duplex connection with a single
// line and no additional hardware the collision detection must be done at the datalink layer
bool PhysicalLayer::probe_tx_line() {
    uint32_t time = millis();
    uint32_t probe_time = time;

    while(probe_time - time < 50) {
        uint8_t result = digitalRead(_tx_pin);
        PrintUtl.prints("Digital read for %d returns %d\n", probe_time - time, result);
        if (!result) {
            return false;
        }
        probe_time = millis();
    }
    return true;
}

// set the tx pin to input and activate the pullup resistor so it will mantain HIGH
void PhysicalLayer::set_tx_pin_to_input() {
    pinMode(_tx_pin, INPUT_PULLUP);
}

void PhysicalLayer::set_tx_pin_to_output() {
    pinMode(_tx_pin, OUTPUT);
    digitalWrite(_tx_pin, HIGH);
}


