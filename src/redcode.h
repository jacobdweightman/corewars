/* Copyright 2018 Jacob Weightman
 *
 * This file is part of corewars-1984.
 *
 * corewars-1984 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * corewars-1984 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jacob Weightman <jacobdweightman@gmail.com>
 */

#ifndef REDCODE_H
#define REDCODE_H

#define INVALID 0
#define DAT 1
#define MOV 2
#define ADD 3
#define SUB 4
#define JMP 5
#define JMZ 6
#define DJZ 7
#define CMP 8
#define IMMEDIATE 9
#define INDIRECT 11
#define DEC 10
#define HEX 16

#define MAX_MNEMONIC CMP

/*
int OPERAND_COUNT[MAX_MNEMONIC];
OPERAND_COUNT[DAT] = 1;
OPERAND_COUNT[MOV] = 2;
OPERAND_COUNT[ADD] = 2;
OPERAND_COUNT[SUB] = 2;
OPERAND_COUNT[JMP] = 1;
OPERAND_COUNT[JMZ] = 2;
OPERAND_COUNT[DJZ] = 2;
OPERAND_COUNT[CMP] = 2;*/

#endif
