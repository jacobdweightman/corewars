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

#ifndef COREWARS_1984_PROGRAM_H_
#define COREWARS_1984_PROGRAM_H_

#include <stdint.h>
#include <stdbool.h>

#define INSTRUCTION_TYPE_WIDTH 4
#define ADDRESSING_MODE_WIDTH 2
#define OPERAND_WIDTH 12

#define IMMEDIATE_MODE 0x0
#define RELATIVE_MODE 0x1
#define INDIRECT_MODE 0x2

#define DAT_TYPE 0
#define MOV_TYPE 1
#define ADD_TYPE 2
#define SUB_TYPE 3
#define JMP_TYPE 4
#define JMZ_TYPE 5
#define DJZ_TYPE 6
#define CMP_TYPE 7

#define MAX_PROGRAM_SIZE 256

#define TYPE_OFFSET (2 * OPERAND_WIDTH + 2 * ADDRESSING_MODE_WIDTH)
#define A_MODE_OFFSET (2 * OPERAND_WIDTH + ADDRESSING_MODE_WIDTH)
#define B_MODE_OFFSET (2 * OPERAND_WIDTH)
#define A_OFFSET OPERAND_WIDTH
#define B_OFFSET 0

#define MODE_MASK ((1 << ADDRESSING_MODE_WIDTH) - 1)
#define OPERAND_MASK ((1 << OPERAND_WIDTH) - 1)

#define TYPE_MASK (((1 << INSTRUCTION_TYPE_WIDTH) - 1) << TYPE_OFFSET)
#define A_MODE_MASK (MODE_MASK << A_MODE_OFFSET)
#define B_MODE_MASK (MODE_MASK << B_MODE_OFFSET)
#define A_MASK (OPERAND_MASK << A_OFFSET)
#define B_MASK (OPERAND_MASK << B_OFFSET)

typedef struct instruction {
    unsigned int type: INSTRUCTION_TYPE_WIDTH;
    unsigned int a_mode: ADDRESSING_MODE_WIDTH;
    unsigned int b_mode: ADDRESSING_MODE_WIDTH;
    unsigned int a: OPERAND_WIDTH;
    unsigned int b: OPERAND_WIDTH;
} instruction;

typedef struct operand {
    unsigned int mode: ADDRESSING_MODE_WIDTH;
    unsigned int value: OPERAND_WIDTH;
} operand;

typedef uint32_t opcode;

typedef struct program {
    int player_id;
    bool alive;
    int PC;
    int size;
    opcode code[MAX_PROGRAM_SIZE];
} program;

#endif
