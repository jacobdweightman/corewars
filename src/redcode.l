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
    #include "program.h"
    #include "y.tab.h"

    char* eoi;
%}

%option caseless
%option yylineno

%%

dat                 return DAT;
mov                 return MOV;
add                 return ADD;
sub                 return SUB;
jmp                 return JMP;
jmz                 return JMZ;
djz                 return DJZ;
cmp                 return CMP;
-{0,1}[0-9]+        { yylval.integer = strtol(yytext, &eoi, 10) & OPERAND_MASK; return INTEGER; }
0x[0-9a-f]+         { yylval.integer = strtol(yytext, &eoi, 16) & OPERAND_MASK; return INTEGER; }
[ \t\n]+            ;
\/\/[^\n]*          ;
.                   { return yytext[0]; }

%%

int yywrap(void) {
    return 1;
}
