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

#ifndef FRAMECRC_H
#define FRAMECRC_H

#include "Frame.h"

class FrameCrc {
    public:
        FrameCrc() {};
        // calculates the CRC for a given frame according to the following rule:
        // The 2's complement of the sum of all the bytes in the frame (including 
        // the CRC byte that should be zero before it is set) 
        uint8_t calculate(Frame * frame);	
        // validates the crc by summing the entire frame
        // the result should be zero
        bool validate(Frame * frame);	
};

extern FrameCrc Crc;

#endif
