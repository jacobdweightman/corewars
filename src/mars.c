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
unsigned int randuint() {
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

    instr.type = (op & TYPE_MASK) >> TYPE_OFFSET;
    instr.a_mode = (op & A_MODE_MASK) >> A_MODE_OFFSET;
    instr.b_mode = (op & B_MODE_MASK) >> B_MODE_OFFSET;
    instr.a = (op & A_MASK) >> A_OFFSET;
    instr.b = (op & B_MASK) >> B_OFFSET;

    return instr;
}

/* Loads the given program into core memory of the given mars. Memory is divided
 * into blocks of MAX_PROGRAM_SIZE, and programs are loaded into a random memory
 * block at a random offset between 0 and MAX_PROGRAM_SIZE - prog->size. */
void load_program(mars* m, program* prog) {
    int block = 0;
    while(m->blocks[block]) {
        block = randuint() % MEMORY_BLOCKS;
    }

    m->blocks[block] = true;

    int offset = 0; //TODO: randuint() % (MAX_PROGRAM_SIZE - prog->size);

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

    if(f) {
        const int size = sizeof(opcode) / sizeof(uint8_t);
        uint8_t data[sizeof(opcode) / sizeof(uint8_t)];

        // read every 4 bytes into code in proper byte order
        while(fread(data, sizeof(uint8_t), 4, f)) {
            opcode op = 0;

            for(int i=0; i<size; i++) {
                op |= data[i] << (8 * (size - i - 1));
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
            return m->core[(index+value) % CORE_SIZE];
        case INDIRECT_MODE:
            return m->core[(m->core[(index+value) % CORE_SIZE]) % CORE_SIZE];
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
            return (index + m->core[(index + value) % CORE_SIZE]) % CORE_SIZE;
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
            m->core[b_addr] = a;
            break;
        case ADD_TYPE:
            m->core[b_addr] += a;
            break;
        case SUB_TYPE:
            m->core[b_addr] -= a;
            break;
        case JMP_TYPE:
            prog->PC = b_addr - 1;
            break;
        case JMZ_TYPE:
            if(a == 0)
                prog->PC = b_addr - 1;
            break;
        case DJZ_TYPE:
            if(--a == 0)
                prog->PC = b_addr - 1;
            break;
        case CMP_TYPE:
            if(a != b)
                (prog->PC)++;
            break;
        default:
            printf("addr %d invalid instruction: %x\n", addr, m->core[addr]);
            return false;
    }

    prog->PC = abs(prog->PC + 1) % CORE_SIZE;
    return true;
}

/* Initializes a new memory array redcode simulator without any programs. */
mars create_mars() {
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

int main() {
    mars m = create_mars();

    program p = read_program(stdin);
    load_program(&m, &p);

    printf("programs: %d\n", m.program_count);
    printf("alive: %d\n", m.alive);

    play(&m);

    print_block(&m, 0);

    return 0;
}
