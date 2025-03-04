%{
#include <stdio.h>
extern int yylex();
extern void yyerror(char*);
extern int yywrap();
%}

// terminal symbols
%token LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET

// non-terminal symbols
%type S T P

// start symbol
%start S

%%
S   :   /* empty */
    |   LEFT_BRACKET T RIGHT_BRACKET S
    |   LEFT_PAREN S RIGHT_PAREN S
    ; 
T   :   P
    |   LEFT_PAREN T RIGHT_PAREN T
    |   LEFT_BRACKET T RIGHT_BRACKET T
    ;
P   :   /* empty */
    |   '(' P
    ;
%%

int main() {
    if (yyparse() == 0) {
        printf("Accepted\n");
    } else {
        printf("Rejected\n");
    }
    return 0;
}

void yyerror(char *s) {
  printf("error\n");
}

int yywrap() {
  return 1;
}