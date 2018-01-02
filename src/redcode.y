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
    #include <stdlib.h>
    #include <stdint.h>

    #include "program.h"

    extern int yylex();

    opcode generate_opcode(unsigned int type, operand* A, operand* B);
    void yyerror(const char* s);
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

%token <integer> IMMEDIATE
%token <integer> INDIRECT

%type <op> operand
%type <op> relind_operand
%type <op> imm_operand
%type <op> rel_operand
%type <op> ind_operand

%%

program         :
                | program instruction
;

instruction     : DAT imm_operand                   { printf("%X\n", generate_opcode(0, NULL, &$2)); }
                | MOV operand relind_operand        { printf("%X\n", generate_opcode(1, &$2, &$3)); }
                | ADD operand relind_operand        { printf("%X\n", generate_opcode(2, &$2, &$3)); }
                | SUB operand relind_operand        { printf("%X\n", generate_opcode(3, &$2, &$3)); }
                | JMP relind_operand                { printf("%X\n", generate_opcode(4, NULL, &$2)); }
                | JMZ operand relind_operand        { printf("%X\n", generate_opcode(5, &$2, &$3)); }
                | DJZ relind_operand relind_operand { printf("%X\n", generate_opcode(6, &$2, &$3)); }
                | CMP operand operand               { printf("%X\n", generate_opcode(7, &$2, &$3)); }
;

operand         : imm_operand
                | rel_operand
                | ind_operand
;

relind_operand  : rel_operand
                | ind_operand
;

imm_operand     : IMMEDIATE INTEGER     {
                                            operand op;
                                            op.mode = IMMEDIATE_MODE;
                                            op.value = $2;
                                            $$ = op;
                                        }
;

rel_operand     : INTEGER               {
                                            operand op;
                                            op.mode = RELATIVE_MODE;
                                            op.value = $1;
                                            $$ = op;
                                        }
;

ind_operand     : INDIRECT INTEGER      {
                                            operand op;
                                            op.mode = INDIRECT_MODE;
                                            op.value = $2;
                                            $$ = op;
                                        }
;

%%

/* Converts an instruction into an opcode. */
opcode generate_opcode(unsigned int type, operand* A, operand* B) {
    opcode result = 0;
    unsigned int bit = 0;

    result |= (B->value << bit);
    bit += OPERAND_WIDTH;

    if(A != NULL) {
        result |= (A->value << bit);
    }
    bit += OPERAND_WIDTH;

    result |= (B->mode << bit);
    bit += ADDRESSING_MODE_WIDTH;

    if(A != NULL) {
        result |= (A->mode << bit);
    }
    bit += ADDRESSING_MODE_WIDTH;

    result |= (type << bit);

    return result;
}

int main() {
    return yyparse();
}

void yyerror(const char* s) {
    fprintf(stderr, "%s\n", s);
    exit(1);
}
