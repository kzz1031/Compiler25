%{
#include <stdio.h>
extern int yylex();
extern void yyerror(char*);
extern int yywrap();
%}

%token RE

%type S

%start S

%%
S : RE;
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
  // do nothing
}

int yywrap() {
  return 1;
}