#include <stdio.h>

extern int yylex();
extern int yylineno;
extern char* yytext;

int main() {
    int token = 1;

    while(token) { // until EOF or invalid token
        token = yylex();
        printf("%d\n", token);
    }

    return 0;
}
