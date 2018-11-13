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

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#include "mars.h"

/* Returns a random unsigned integer from /dev/urandom. */
unsigned int randuint(void) {
    unsigned int val;

    FILE* f = fopen("/dev/urandom", "r");
    fread(&val, sizeof(val), 1, f);
    fclose(f);

    return val;
}

/* Prints the hex values stored in each memory location of the mars in the given
 * block of core memory to stdout. */
void print_block(mars* m, int index) {
    int base_index = MAX_PROGRAM_SIZE * index;

    for(int i=0; i<MAX_PROGRAM_SIZE / 8; i++) {
        for(int j=0; j<8; j++) {
            printf("%08x ", m->core[base_index + 8*i + j]);
        }

        printf("\n");
    }
}

/* Creates an instruction struct and populates its fields with information
 * encoded in the given opcode. */
instruction get_instruction(opcode op) {
    instruction instr;

    instr.type = (op & OP_TYPE_MASK) >> TYPE_OFFSET;
    instr.a_mode = (op & A_MODE_MASK) >> A_MODE_OFFSET;
    instr.b_mode = (op & B_MODE_MASK) >> B_MODE_OFFSET;
    instr.a = (op & A_MASK) >> A_OFFSET;
    instr.b = (op & B_MASK) >> B_OFFSET;

    return instr;
}

/* Chooses a random, unoccupied block into which to load a program, marks
 * it as occupied, and returns its number. If the core is full, it will
 * return -1. Note that this function will likely run slowly when a large
 * fraction of blocks are occupied. */
int get_block(mars* m) {
    // if core is full, return -1
    bool full = true;

    for(int i=0; i<MEMORY_BLOCKS; i++) {
      if(!m->blocks[i]) {
        full = false;
        break;
      }
    }

    if(full) {
      return -1;
    }

    // pick a random free block
    int block;

    do {
        block = randuint() % MEMORY_BLOCKS;
    } while(m->blocks[block]);

    m->blocks[block] = true;

    return block;
}

/* Returns a random offset within a block such that the program fits between
 * block_base + offset and block_base + block_size. */
unsigned int get_offset(program* prog) {
    return randuint() % (unsigned int)(MAX_PROGRAM_SIZE - prog->size + 1);
}

/* Loads the given program into the given block of core memory of the given
 * mars. Memory is divided into blocks of MAX_PROGRAM_SIZE, and programs are
 * loaded into a random memory block at a random offset between 0 (inclusive)
 * and MAX_PROGRAM_SIZE - prog->size (inclusive). */
void load_program(mars* m, program* prog, int block, int offset) {
    int base_index = block * MAX_PROGRAM_SIZE + offset;

    for(int i=0; i<prog->size; i++) {
        m->core[base_index+i] = prog->code[i];
    }

    prog->PC = base_index;

    m->programs[m->program_count] = prog;
    (m->program_count)++;
    (m->alive)++;
}

/* Reads a program from the given file handle into a program struct. */
program read_program(FILE* f) {
    program prog;
    prog.player_id = 0;
    prog.alive = true;
    prog.PC = 0;
    prog.size = 0;

    memset(prog.code, 0, sizeof(prog.code));

    if(f) {
        const int size = sizeof(opcode) / sizeof(uint8_t);
        uint8_t data[sizeof(opcode) / sizeof(uint8_t)];

        // read every 4 bytes into code in little-endian byte order
        while(fread(data, sizeof(uint8_t), 4, f)) {
            opcode op = 0;

            for(int i=0; i<size; i++) {
                op |= ((opcode) data[i] << (8*i));
            }

            prog.code[prog.size] = op;
            prog.size++;
        }
    } else {
        printf("You must provide an open file from which to read the program.\n");
        prog.player_id = -1;
    }

    return prog;
}

/* Returns as a signed int the value of an operand from an instruction at the
 * given address, with the given addressing mode, and with the given value.
 * This function assumes the given value occupies only its rightmost 12 bits. */
int get_operand_value(mars* m, int index, unsigned int mode,
                      unsigned int raw_value) {
    int value = get_signed_operand_value(raw_value);

    switch (mode) {
        case IMMEDIATE_MODE:
            return value;
        case RELATIVE_MODE:
            raw_value = m->core[(index+value) % CORE_SIZE];
            return get_signed_operand_value(raw_value);
        case INDIRECT_MODE:
            raw_value = m->core[(index+value) % CORE_SIZE];
            raw_value = m->core[raw_value % CORE_SIZE];
            return get_signed_operand_value(raw_value);
        default:
            printf("died: invalid addressing mode\n");
            return 0xFFFF;
    }
}

/* Returns the index into core memory pointed to by the given operand. */
int get_operand_address(mars* m, int index, unsigned int mode,
                        unsigned int raw_value) {
    int value = get_signed_operand_value(raw_value);

    switch (mode) {
        case RELATIVE_MODE:
            return (index + value) % CORE_SIZE;
        case INDIRECT_MODE:
            return (index + (int)(m->core[(index + value) % CORE_SIZE])) % CORE_SIZE;
        default:
            return -1;
    }
}

/* Executes the next instruction for the given program. Returns false if the
 * instruction was invalid. */
bool tick(mars* m, program* prog) {
    int addr = prog->PC;
    instruction instr = get_instruction(m->core[addr]);
    printf("addr: %d, value: %x\n", addr, m->core[addr]);

    int a = get_operand_value(m, addr, instr.a_mode, instr.a);
    int b = get_operand_value(m, addr, instr.b_mode, instr.b);
    int b_addr = get_operand_address(m, addr, instr.b_mode, instr.b);

    switch (instr.type) {
        case MOV_TYPE:
            printf("MOV\n");
            m->core[b_addr] = (opcode) a;
            break;
        case ADD_TYPE:
            printf("ADD\n");
            m->core[b_addr] = (opcode) ((int) m->core[b_addr] + a); // TODO: do this better
            break;
        case SUB_TYPE:
            printf("SUB\n");
            m->core[b_addr] = (opcode) ((int) m->core[b_addr] - a); // TODO: do this better
            break;
        case JMP_TYPE:
            printf("JMP\n");
            prog->PC = b_addr - 1;
            break;
        case JMZ_TYPE:
            printf("JMZ\n");
            if(a == 0)
                prog->PC = b_addr - 1;
            break;
        case DJZ_TYPE:
            printf("DJZ\n");
            if(--a == 0)
                prog->PC = b_addr - 1;
            break;
        case CMP_TYPE:
            if(a != b)
                (prog->PC)++;
            break;
        default:
            printf("uh oh... %d\n", instr.type);
            printf("type: %x modeA: %x modeB: %x opA: %x opB: %x\n", instr.type, instr.a_mode, instr.b_mode, instr.a, instr.b);
            printf("addr %d invalid instruction: %x\n", addr, m->core[addr]);
            return false;
    }

    prog->PC = abs(prog->PC + 1) % CORE_SIZE;
    return true;
}

/* Initializes a new memory array redcode simulator without any programs. */
mars create_mars(void) {
    mars m;
    m.program_count = 0;
    m.alive = 0;
    m.elapsed = 0;
    memset(m.core, 0, sizeof(m.core));
    memset(m.blocks, false, sizeof(m.blocks));

    return m;
}

/* Carries out gameplay on the given mars until the game duration is met or no
 * all programs have terminated. Returns the player_id of the winning program.
 */
int play(mars* m) {
    while(m->elapsed < DURATION && m->alive > 0) {
        for(int j=0; j<m->program_count; j++) {
            if(m->programs[j]->alive) {
                if(!tick(m, m->programs[j])) {
                    m->programs[j]->alive = false;
                    (m->alive)--;
                }
            }
        }
    }

    return 0; // TODO: return index of winning program
}
