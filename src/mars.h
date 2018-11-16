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

#include <stdbool.h>

#include "program.h"

typedef struct warrior {
    unsigned int id;
    unsigned int PC;
    struct warrior* prev;
    struct warrior* next;
} warrior;

typedef struct mars {
    unsigned int core_size;
    unsigned int block_size;
    unsigned int duration;
    unsigned int elapsed;
    unsigned int alive_count;
    warrior* next_warrior;
    opcode* core;
    bool* blocks;
} mars;

void destroy_mars(mars* m);
mars create_mars(unsigned int core_size, unsigned int block_size, unsigned int duration);
warrior load_program(mars* m, program* prog, unsigned int block, unsigned int offset);
unsigned int get_block(mars* m);
unsigned int get_offset(mars* m, program* prog);
void tick(mars* m);
int play(mars* m);

// DEBUG FUNCTIONS
void print_block(mars* m, unsigned int index);

#ifdef TEST_BUILD
void insert_warrior(mars* m, warrior* w);
void remove_warrior(mars* m, warrior* w);
int get_operand_value(mars* m, int index, unsigned int mode, unsigned int raw_value);
int get_operand_address(mars* m, int index, unsigned int mode, unsigned int raw_value);
#endif

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
}

#endif
