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
#include <limits.h>

#include "program.h"

/* Creates an instruction struct and populates its fields with information
 * encoded in the given opcode.
 *
 * @param op - the opcode to be decoded
 * @return a populated instruction struct */
instruction decode(opcode op) {
    instruction instr;

    instr.type = (op & OP_TYPE_MASK) >> TYPE_OFFSET;
    instr.a_mode = (op & A_MODE_MASK) >> A_MODE_OFFSET;
    instr.b_mode = (op & B_MODE_MASK) >> B_MODE_OFFSET;
    instr.a = (op & A_MASK) >> A_OFFSET;
    instr.b = (op & B_MASK) >> B_OFFSET;

    return instr;
}

/* Deallocates dynamically allocated memory held by the given program to prevent
 * memory leaks. This function must be called before a program falls out of
 * scope or is freed.
 *
 * @param prog the program to clean up */
void destroy_program(program* prog) {
    free(prog->code);
}

/* Reads a program from the given opcode buffer. Since a valid program may
 * contain null bytes, the size of the program must be included so that it can
 * be properly read. Note that an improper size may result in buffer
 * overflow when the program is loaded into a mars instance.
 *
 * @param id - an identification number of the player that owns this program
 * @param buf - a pointer to a buffer of opcodes containing the program source
 * @param size - the size of the program, in number of opcodes
 * @return a program */
program prog_from_buffer(unsigned int id, opcode* buf, unsigned long size) {
    program prog;
    prog.code = buf;
    prog.size = size;
    prog.id = id;

    return prog;
}

/* Reads a program from the given file. The returned program will have an id of
 * UINT_MAX if an error occurred.
 *
 * @param id - an identification number of the player that owns this program
 * @param f - a handle on the file from which to read the program
 * @return a program */
program prog_from_file(unsigned int id, FILE* f) {
    program prog;

    fseek(f, 0L, SEEK_END);
    unsigned long length = (unsigned long) ftell(f);
    fseek(f, 0L, SEEK_SET);

    if(length % sizeof(opcode) != 0) {
        // source file ends with a partial opcode
        prog.id = UINT_MAX;
        return prog;
    }

    opcode* code = (opcode*) malloc(length);

    int i = 0;
    if(f) {
        unsigned char word[sizeof(opcode)];

        while(fread(word, 1, sizeof(opcode), f) == sizeof(opcode)) {
            opcode op = 0;

            for(unsigned int j=0; j<sizeof(opcode); j++) {
              op |= ((opcode) word[j] << (8*j));
            }

            code[i] = op;
            i++;
        }
    } else {
      // file I/O problem
      prog.id = UINT_MAX;
      prog.size = 0;
      return prog;
    }

    prog.id = id;
    prog.code = code;
    prog.size = length / 4;

    return prog;
}
