%{
#include <stdio.h>
extern int yylex();
extern void yyerror(char*);
extern int yywrap();
%}

// terminal symbols
%token LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET

// non-terminal symbols
%type S P B I

// start symbol
%start S

%%
S   :   /* empty */
    |   P S
    |   B S
    ; 
P   :   LEFT_PAREN S RIGHT_PAREN
    ;
B   :   LEFT_BRACKET I RIGHT_BRACKET
I   :   /* empty */
    |   P I
    |   B I
    |   LEFT_PAREN I
%%

int main() {
    if (yyparse() == 0) {
        printf("accept\n");
    } else {
        printf("reject\n");
    }
    return 0;
}

void yyerror(char *s) {
  printf("error\n");
}

int yywrap() {
  return 1;
}