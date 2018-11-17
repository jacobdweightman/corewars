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
#include <stdbool.h>
#include <limits.h>

#include "mars.h"
#include "utils.h"

/* Prints the hex values stored in each memory location of the mars in the given
 * block of core memory to stdout. */
void print_block(mars* m, unsigned int index) {
    unsigned int base_index = m->block_size * index;

    for(unsigned int i=0; i<m->block_size / 8 + 1; i++) {
        for(unsigned int j=0; j<8 && j<m->block_size; j++) {
            printf("%08x ", m->core[base_index + 8*i + j]);
        }

        printf("\n");
    }
}

/* Deallocates dynamically allocated memory held by the given mars to prevent
 * memory leaks. This function must be called before a mars falls out of
 * scope or is freed.
 *
 * @param m - the mars to clean up */
void destroy_mars(mars* m) {
    free(m->core);
}

/* Initializes a new, empty Memory Array Redcode Simulator (MARS) with the given
 * properties.
 *
 * @param core_size - the size of the MARS array, measured in opcodes
 * @param block_size - the maximum permissible size of a program
 * @param duration - the number of ticks before the game is declared a draw
 * @return a new MARS */
mars create_mars(unsigned int core_size, unsigned int block_size, unsigned int duration) {
    mars m;

    m.core_size = core_size;
    m.block_size = block_size;
    m.duration = duration;
    m.elapsed = 0;
    m.alive_count = 0;
    m.next_warrior = NULL;
    m.core = (opcode*) malloc(sizeof(opcode) * core_size);
    m.blocks = (bool*) malloc(sizeof(bool) * core_size / block_size);

    memset(m.core, 0, sizeof(opcode) * core_size);
    memset(m.blocks, 0, sizeof(bool) * core_size / block_size);

    return m;
}

/* Inserts the given warrior into the mars warrior list, so that it will take
 * turns executing instructions on the mars. The warrior is inserted after the
 * next warrior to take its turn, and becomes the next warrior to run. The mars
 * warrior counter is also incremented.
 *
 * @param m - the mars to which the warrior is being added
 * @param w - the warrior to add to the mars */
void insert_warrior(mars* m, warrior* w) {
    if(m->next_warrior == NULL) {
        w->next = w;
        w->prev = w;
    } else {
        // circular doubly-linked list insert
        warrior* prev = m->next_warrior;
        warrior* next = prev->next;

        prev->next = w;
        w->prev = prev;
        next->prev = w;
        w->next = next;
    }

    m->next_warrior = w;
    (m->alive_count)++;
}

/* Removes the given warrior into the mars warrior list, so that it will no
 * longer take turns executing instructions on the mars. If the warrior to be
 * removed is currently the next to run on the mars, the next to run will change
 * to the one after. The mars warrior counter is also decremented.
 *
 * @param m - the mars from which the warrior is being removed
 * @param w - the warrior to remove from the mars */
void remove_warrior(mars* m, warrior* w) {
    warrior* prev = w->prev;
    warrior* next = w->next;
    prev->next = next;
    next->prev = prev;

    // the removed warrior cannot be the next to run!
    if(m->next_warrior == w) {
        // corner case: w is the only warrior in the mars
        if(m->alive_count == 1) {
            m->next_warrior = NULL;
        } else {
            m->next_warrior = w->next;
        }
    }

    (m->alive_count)--;
}

/* Creates a new warrior in the given mars, starting with the code of the given
 * program. Note that the given block must be free--this method does not check
 * whether it is free, and will overwrite another program that is loaded there.
 * Offset must also be less than m->block_size - prog.size
 *
 * @param m - the mars to which the new warrior should be added
 * @param prog - a program with the original code for the new warrior
 * @param block - the block number in the mars into which to load code
 * @param offset - the offset within the given block at which to load code
 * @return the newly created warrior, which has been loaded into m */
warrior load_program(mars* m, program* prog, unsigned int block, unsigned int offset) {
    warrior w;
    w.id = prog->id;

    insert_warrior(m, &w);

    // load program into mars memory
    unsigned int base = m->block_size * block + offset;
    w.PC = base;

    for(unsigned int i=0; i<prog->size; i++) {
        m->core[base+i] = prog->code[i];
    }

    return w;
}

/* Chooses a random, unoccupied block into which to load a program, marks
 * it as occupied, and returns its number. If the core is full, it will
 * return UINT_MAX. Note that this function will likely run slowly when a large
 * fraction of blocks are occupied. Since the returned block is guaranteed to be
 * unoccupied, the return value of this function is safe to pass
 * into load_program.
 *
 * @return index of a free block in the given mars */
unsigned int get_block(mars* m) {
    // if core is full, return -1
    bool full = true;

    for(unsigned int i=0; i<(m->core_size / m->block_size); i++) {
      if(!m->blocks[i]) {
        full = false;
        break;
      }
    }

    if(full) {
      return UINT_MAX;
    }

    // pick a random free block
    unsigned int block;

    do {
        block = randuint() % (m->core_size / m->block_size + 1);
    } while(m->blocks[block]);

    m->blocks[block] = true;

    return block;
}

/* Chooses a random offset within a block such that the program fits between
 * block_base + offset and block_base + block_size. The return value of this
 * function is safe to pass into load_program.
 *
 * @return safe offset for the given program within a block */
unsigned int get_offset(mars* m, program* prog) {
    return randuint() % (unsigned int)(m->block_size - prog->size + 1);
}

/* Returns as a signed int the value of an operand from an instruction at the
 * given address, with the given addressing mode, and with the given value.
 * This function assumes the given value occupies only its rightmost 12 bits.
 * If this operand is invalid, INT_MAX is returned instead.
 *
 * @param m - the mars which should be referenced for relative/indirect modes
 * @param index - the location of the instruction, so offsets may be calculated
 * @param mode - the addressing mode of the operand
 * @param raw_value - the right-aligned 12-bit value encoding the value */
int get_operand_value(mars* m, int index, unsigned int mode,
                      unsigned int raw_value) {
    int value = get_signed_operand_value(raw_value);

    switch (mode) {
        case IMMEDIATE_MODE:
            return value;
        case RELATIVE_MODE:
            raw_value = m->core[wrap_index(index+value, m->core_size)];
            return (int) raw_value;
        case INDIRECT_MODE:
            raw_value = m->core[wrap_index(index+value, m->core_size)];
            value = (int) raw_value;
            raw_value = m->core[wrap_index(index+value, m->core_size)];
            return (int) raw_value;
        default:
            printf("died: invalid addressing mode\n");
            return INT_MAX;
    }
}

/* Compute the address refered to by an operand in the designated addressing
 * mode. This address is an index into the mars core. If the mode does not give
 * a valid address, INT_MAX will be returned.
 *
 * @param m - the mars whose memory will be referenced
 * @param index - the address of the executing instruction
 * @param mode - the addressing mode of the executing instruction
 * @param raw_value - the operand value bitstring extracted from the instruction
 * @return the index into core memory pointed to by the given operand */
int get_operand_address(mars* m, int index, unsigned int mode,
                        unsigned int raw_value) {
    int value = get_signed_operand_value(raw_value);
    int addr = wrap_index(index + value, m->core_size);

    switch (mode) {
        case RELATIVE_MODE:
            return addr;
        case INDIRECT_MODE:
            value = (int) m->core[addr];
            return wrap_index(index + value, m->core_size);
        default:
            return INT_MAX;
    }
}

/* Executes the next instruction for the given program. */
void tick(mars* m) {
    warrior* prog = m->next_warrior; // does this fix it?

    int addr = (int) prog->PC;
    instruction instr = decode(m->core[addr]);
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
            prog->PC = (unsigned int) (b_addr - 1) % m->core_size;
            break;
        case JMZ_TYPE:
            printf("JMZ\n");
            if(a == 0)
                prog->PC = (unsigned int) (b_addr - 1) % m->core_size;
            break;
        case DJZ_TYPE:
            printf("DJZ\n");
            if(--a == 0)
                prog->PC = (unsigned int) (b_addr - 1) % m->core_size;
            break;
        case CMP_TYPE:
            if(a != b)
                prog->PC = (prog->PC + 1) % m->core_size;
            break;
        default:
            printf("uh oh... %d\n", instr.type);
            printf("type: %x modeA: %x modeB: %x opA: %x opB: %x\n", instr.type, instr.a_mode, instr.b_mode, instr.a, instr.b);
            printf("addr %d invalid instruction: %x\n", addr, m->core[addr]);

            remove_warrior(m, prog);
    }

    prog->PC = (prog->PC + 1) % m->core_size;
    m->next_warrior = m->next_warrior->next;
    m->elapsed++;
}

/* Carries out gameplay on the given mars until the game duration is met or only
 * one program is still running. Returns the player_id of the winning program,
 * or -1 if there is a draw.
 */
int play(mars* m) {
    while(m->elapsed < m->duration && m->alive_count > 0) {
      tick(m);
    }

    return 0; // TODO: return index of winning program
}
