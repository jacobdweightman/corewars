%{
    #include "redcode.h"
%}

%option caseless

%%

dat                 return DAT;
mov                 return MOV;
add                 return ADD;
sub                 return SUB;
jmp                 return JMP;
jmz                 return JMZ;
djz                 return DJZ;
cmp                 return CMP;
#                   return IMMEDIATE;
@                   return INDIRECT;
-{0,1}[0-9]+        return DEC;
0x[0-9a-f]+         return HEX;
[ \t\n]+            ;
\/\/[^\n]*          ;
.                   return INVALID;

%%

int yywrap(void) {
    return 1;
}
