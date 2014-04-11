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
#include "FrameCrc.h"
#include <stdio.h>

uint8_t FrameCrc::calculate(Frame * frame) {

    uint8_t * frame_pointer = (uint8_t *) frame;
    uint8_t crc = 0;
    uint8_t bytes_sum = 0;

    for(int i = 0; i < sizeof(*frame) - 1; i++, frame_pointer++) {
        bytes_sum = (bytes_sum + *frame_pointer) & 0xFF;
    }

    crc = (~bytes_sum + 1) & 0xFF;

    return crc;
}

bool FrameCrc::validate(Frame * frame) {

    uint8_t * frame_pointer = (uint8_t *) frame;
    uint8_t bytes_sum = 0;

    for(int i = 0; i < sizeof(*frame); i++, frame_pointer++) {
        bytes_sum = (bytes_sum + *frame_pointer) & 0xFF;
    }
    uint8_t crc = frame->get_crc();
    return bytes_sum == 0;
}

FrameCrc Crc = FrameCrc();
