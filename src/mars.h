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

#define DURATION 84
#define CORE_SIZE 257//4096
#define MEMORY_BLOCKS CORE_SIZE / MAX_PROGRAM_SIZE

typedef struct mars {
    int program_count;
    int alive;
    int elapsed;
    program* programs[MEMORY_BLOCKS];
    opcode core[CORE_SIZE];
    bool blocks[MEMORY_BLOCKS];
} mars;

unsigned int randuint(void);
void print_block(mars* m, int index);
instruction get_instruction(opcode op);
void load_program(mars* m, program* prog, int block, int offset);
program read_program(FILE* f);
int get_operand_value(mars* m, int index, unsigned int mode,
                      unsigned int raw_value);
int get_operand_address(mars* m, int index, unsigned int mode,
                        unsigned int raw_value);
bool tick(mars* m, program* prog);
mars create_mars(void);
int play(mars* m);

/* Converts an unsigned 12-bit value into a signed int. */
static inline int get_signed_operand_value(unsigned int raw_value) {

    int value;

    if(raw_value & (1 << (OPERAND_WIDTH - 1))) { // value is negative (MSB is 1)
        raw_value = (~raw_value + 1) & OPERAND_MASK; // get magnitude
        value = (int) raw_value;
        return -value;
    } else {
        return (int) raw_value;
    }


    /*int value = raw_value;

    if(raw_value & (1 << (OPERAND_WIDTH - 1))) { // value is negative (MSB is 1)
        value ^= (1 << (OPERAND_WIDTH - 1));
        value -= 0x800;
    }

    return value;*/
}

#endif
