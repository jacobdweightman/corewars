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

%{
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "program.h"

#define BITMASK_8 ((unsigned int) (1 << 8) - 1)

extern int yylineno;
extern FILE* yyin;

extern int yylex();
extern int yyparse();

void yyerror(const char* s);

opcode output[MAX_PROGRAM_SIZE];
int instruction_index = 0;

opcode generate_opcode(unsigned int type, operand* A, operand* B);
void write_instruction(unsigned int type, operand* A, operand* B);
%}

%start program
%union {
    operand op;
    int integer;
}

%token DAT
%token MOV
%token ADD
%token SUB
%token JMP
%token JMZ
%token DJZ
%token CMP

%token <integer> INTEGER

%type <op> operand
%type <op> relind_operand
%type <op> imm_operand
%type <op> rel_operand
%type <op> ind_operand

%%

program         :
                | program instruction
;

instruction     : DAT imm_operand                   { write_instruction(DAT_TYPE, NULL, &$2); }
                | MOV operand relind_operand        { write_instruction(MOV_TYPE, &$2, &$3); }
                | ADD operand relind_operand        { write_instruction(ADD_TYPE, &$2, &$3); }
                | SUB operand relind_operand        { write_instruction(SUB_TYPE, &$2, &$3); }
                | JMP relind_operand                { write_instruction(JMP_TYPE, NULL, &$2); }
                | JMZ operand relind_operand        { write_instruction(JMZ_TYPE, &$2, &$3); }
                | DJZ relind_operand relind_operand { write_instruction(DJZ_TYPE, &$2, &$3); }
                | CMP operand operand               { write_instruction(CMP_TYPE, &$2, &$3); }
;

operand         : imm_operand
                | rel_operand
                | ind_operand
;

relind_operand  : rel_operand
                | ind_operand
;

imm_operand     : '#' INTEGER   {
                                    operand op;
                                    op.mode = IMMEDIATE_MODE;
                                    op.value = (unsigned int) $2;
                                    $$ = op;
                                }
;

rel_operand     : INTEGER       {
                                    operand op;
                                    op.mode = RELATIVE_MODE;
                                    op.value = (unsigned int) $1;
                                    $$ = op;
                                }
;

ind_operand     : '@' INTEGER   {
                                    operand op;
                                    op.mode = INDIRECT_MODE;
                                    op.value = (unsigned int) $2;
                                    $$ = op;
                                }
;

%%

/* Converts an instruction into an opcode. */
inline opcode generate_opcode(unsigned int type, operand* A, operand* B) {
    opcode result = 0;

    result |= ((opcode) B->value << B_OFFSET);
    result |= ((opcode) B->mode << B_MODE_OFFSET);

    if(A != NULL) {
        result |= ((opcode) A->value << A_OFFSET);
        result |= ((opcode) A->mode << A_MODE_OFFSET);
    }

    result |= (type << TYPE_OFFSET);

    return result;
}

/* writes an instruction into the output buffer */
void write_instruction(unsigned int type, operand* A, operand* B) {
    output[instruction_index] = generate_opcode(type, A, B);
    instruction_index++;
}

/* Takes an input and output file. Reads a Redcode program from the input,
 * and writes machine code to the output. */
int assemble(FILE* input_stream, FILE* output_stream) {
    memset(output, 0, sizeof(output));
    instruction_index = 0;

    yyin = input_stream;

    int ret = yyparse();

    for(int i=0; i<instruction_index; i++) {
        for(int j=0; j<32; j += 8) { // output is little endian
            uint8_t c = (uint8_t) ((output[i] & (BITMASK_8 << j)) >> j);
            //printf("%c", c);
            fprintf(output_stream, "%c", c);
        }
    }

    fflush(output_stream);

    return ret;
}

void yyerror(const char* s) {
    fprintf(stderr, "%s: line %d\n", s, yylineno);
    exit(1);
}
