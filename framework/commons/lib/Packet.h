/*
   Half-duplex 1-wire connections between multiple Arduino and RPi
   Copyright (C) 2014  Jo„Salavisa (joao.salavisa@gmail.com)

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

#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

class Packet {
    private:
        uint8_t data[32];
    public:
        Packet() { };
        ~Packet() { };
};

#endif /* PACKET_H */
