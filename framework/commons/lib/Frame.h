/*
   Half-duplex 1-wire connections between multiple Arduino and RPi
   Copyright (C) 2014  Joao Salavisa (joao.salavisa@gmail.com)

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
#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>

#define PAYLOAD_MAX_SIZE 		32 /* Size of the payload */

#define CONTROL_FIST_FRAME	0b10000000 /* Control flag to define the first frame in the sequence */
#define CONTROL_LAST_FRAME	0b01000000 /* Control flag to define the last frame in the sequence */
#define CONTROL_ACK_FRAME	0b00100000 /* Control flag to define an ack frame */
#define CONTROL_TRANS_FRAME	0b00000001 /* Control flag to define if a frame was transmitted */
#define CONTROL_ACKED_FRAME	0b00000010 /* Control flag to define if a frame was acked */

class Frame {
    private:
        // id of the frame. The triplet (_frame_id, _src_address, _dst_address) 
        // should be unique for an entire message that is sent
        uint32_t _frame_id;
        // the frame sequence number. 
        uint32_t _frame_seq;
        // the address of the sender
        uint8_t _src_address;
        // the destination address
        uint8_t _dst_address;
        // control data 
        uint8_t _control_data;
        // the payload
        uint8_t _payload[PAYLOAD_MAX_SIZE];
        // the 8 bit checksum of the frame
        uint8_t _crc;
    public:
        Frame();
        Frame(uint32_t frame_id, uint32_t _frame_seq, uint8_t src_address, uint8_t dst_address, uint8_t control_data, uint8_t * payload, uint16_t payload_size);
        uint8_t get_src_address() { return _src_address; };
        uint8_t get_dst_address() { return _dst_address; };
        uint8_t get_control_data() { return _control_data; };
        uint8_t * get_payload() { return _payload; };
        uint8_t get_crc() { return _crc; };
        uint32_t get_frame_id() { return _frame_id; }
        uint32_t get_frame_seq() { return _frame_seq; }

        void set_src_address(uint8_t src_address) { _src_address = src_address; };
        void set_dst_address(uint8_t dst_address) { _dst_address = dst_address; };
        void set_control_data(uint8_t control_data) { _control_data = control_data; };
        void set_payload(uint8_t * payload, uint16_t payload_size);
        void prepare_to_transmit();
        void set_crc(uint8_t crc) { _crc = crc; };

        void print(); 

} __attribute__ ((aligned (1), packed));

#endif /* FRAME_H */
