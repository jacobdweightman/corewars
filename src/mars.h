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

#ifndef COREWARS_1984_MARS_H_
#define COREWARS_1984_MARS_H_

#include "program.h"

#define DURATION 1000
#define CORE_SIZE 257//4096
#define MEMORY_BLOCKS CORE_SIZE / MAX_PROGRAM_SIZE

void print_block(int index);
unsigned int randuint();
instruction get_instruction(opcode op);
void load_program(program* prog);
program read_program(FILE* f);
int get_operand_value(int index, unsigned int mode, unsigned int raw_value);
int get_operand_address(int index, unsigned int mode, unsigned int raw_value);

/* Converts an unsigned 12-bit value into a signed int. */
static inline int get_signed_operand_value(unsigned int raw_value) {
    int value = raw_value;

    if(raw_value & (1 << (OPERAND_WIDTH - 1))) { // value is negative (MSB is 1)
        value ^= (1 << (OPERAND_WIDTH - 1));
        value -= 0x800;
    }

    return value;
}

#endif
