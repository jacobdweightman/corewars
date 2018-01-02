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

#ifndef COREWARS
#define COREWARS

#include <stdint.h>

#define CORE_SIZE 4096
#define MAX_PROGRAM_SIZE 256
#define MEMORY_BLOCKS CORE_SIZE / MAX_PROGRAM_SIZE

#define INSTRUCTION_TYPE_WIDTH 4
#define ADDRESSING_MODE_WIDTH 2
#define OPERAND_WIDTH 12

#define IMMEDIATE_MODE 0b00
#define RELATIVE_MODE 0b01
#define INDIRECT_MODE 0b10

typedef struct instruction {
    unsigned int type: INSTRUCTION_TYPE_WIDTH;
    unsigned int a_mode: ADDRESSING_MODE_WIDTH;
    unsigned int b_mode: ADDRESSING_MODE_WIDTH;
    int a: OPERAND_WIDTH;
    int b: OPERAND_WIDTH;
} instruction;

typedef struct operand {
    unsigned int mode: ADDRESSING_MODE_WIDTH;
    int value: OPERAND_WIDTH;
} operand;

typedef uint32_t opcode;

typedef struct program {
    int player_id;
    opcode* PC;
    int size;
    opcode code[MAX_PROGRAM_SIZE];
} program;

#endif
