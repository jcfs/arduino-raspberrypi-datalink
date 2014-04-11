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

#include "PrintUtil.h"
#include <stdarg.h>
#include <stdio.h>

void PrintUtil::prints(const char * format, ...) {
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap); 
    return;
};

PrintUtil PrintUtl = PrintUtil();
