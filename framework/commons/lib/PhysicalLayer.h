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

#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H

#include <stdint.h>

#define DEFAULT_WAIT_TX_SND 	   200 /* Time to wait until we change the tx pin back to input */
#define MAX_PROBE_TRY 		   10 /* Max probe tries in the tx line */
//Error codes
#define ELBSY 			    0 /* Line busy error */
#define ENAVL 			   -1 /* Not available data */

/*
   Physical Layer interface. 
 */
class PhysicalLayer {

    private:
        // file desciptor that can be use in some implementations
        // (ie: linux implementation Raspberry pi)
        uint32_t _fd;
        uint8_t _tx_pin, _rx_pin;

        //Pointer to the media access object in case it is needed
        //Can be used in arduino implementations that use software serial
        //to handle the line
        void * _media_access;

        // probes the transmission line to know if we are available to transmit
        bool probe_tx_line();
        void set_tx_pin_to_output();
        void set_tx_pin_to_input();
    public:
        // tx_pin : the pin used to transmit data (can be the same as rx)
        // rx_pin : the pin used to receive data (can be the same as tx)
        // device : on some sistems the device os the serial port is needed
        // baud   : the baud rate of the connection
        PhysicalLayer(uint8_t tx_pin, uint8_t rx_pin, char * device, uint32_t baud);

        // write a byte using the tx line if available. If the line is busy a ELBSY	
        // is returned, otherwise the amount of bytes written are returned
        uint32_t write(uint8_t byte, bool block = false);

        // write a buffer of using the tx line if available. If the line is busy a ELBSY	
        // is returned, otherwise the amount of bytes written are returned
        uint32_t write(uint8_t * buffer, uint32_t size, bool block = false);

        // reads a byte of the rx line and returns it
        uint32_t read(uint8_t *byte);

        // reads <code>size<code> bytes if available on the rx line
        uint32_t read(uint8_t * buffer, uint32_t size);

        // returns the amount number of bytes available to read in the rx line
        uint32_t available();

};


#endif /* PHYSICALLAYER_H */
