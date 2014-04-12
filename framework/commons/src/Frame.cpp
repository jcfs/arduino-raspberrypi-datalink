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

#include "Frame.h"
#include "FrameCrc.h"
#include "PrintUtil.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define NO_PAYLOAD 0x00

Frame::Frame() {
    _frame_id = 0; 
    _frame_seq = 0; 
    _src_address = 0; 
    _dst_address = 0; 
    _control_data = 0; 
    _crc = 0;

    for(int i = 0; i < PAYLOAD_MAX_SIZE; i++) {
        _payload[i] = NO_PAYLOAD;
    }
    
}

Frame::Frame(uint32_t frame_id, uint32_t frame_seq, uint8_t src_address, uint8_t dst_address, uint8_t control_data, uint8_t * payload, uint16_t payload_size) {
    _src_address = src_address;
    _dst_address = dst_address;
    _frame_id = frame_id;
    _frame_seq = frame_seq;
    _control_data = control_data;

    set_payload(payload, payload_size);

    _crc = 0;
    _crc = Crc.calculate(this);
}

void Frame::set_payload(uint8_t * payload, uint16_t payload_size) {

    // validate the payload size
    if (payload_size > PAYLOAD_MAX_SIZE) {
        return;
    }

    for(int i = 0; payload_size < payload_size; i++) {
        _payload[i] = *payload;
        payload++;
    }

    // fill the rest with NO_PAYLOAD
    for(int i = payload_size; i < PAYLOAD_MAX_SIZE; i++) {
        _payload[i] = NO_PAYLOAD;
    }
}

void Frame::prepare_to_transmit() {
    _crc = Crc.calculate(this);
}

void Frame::print() {
    PrintUtl.prints("Frame Id=%lu\n\r\tFrame Seq=%lu\n\r\tSrc=%d\n\r\tDest=%d\r\n\tControl=",
            _frame_id,
            _frame_seq,
            _src_address,
            _dst_address);

    if (_control_data & CONTROL_FIST_FRAME) PrintUtl.prints("FST ");
    if (_control_data & CONTROL_LAST_FRAME) PrintUtl.prints("LST ");
    if (_control_data & CONTROL_ACK_FRAME) PrintUtl.prints("ACK ");
    if (_control_data & CONTROL_TRANS_FRAME) PrintUtl.prints("TX ");
    if (_control_data & CONTROL_ACKED_FRAME) PrintUtl.prints("ACKED");
    PrintUtl.prints("\n\r\tControl(n)=%d\n\r\tCrc=%d\r\n", _control_data, _crc);
    PrintUtl.prints("\n\r");
}


