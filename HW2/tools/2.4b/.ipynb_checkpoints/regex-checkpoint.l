/* 1. declarations */

/* included code */
%{

#include "parser.h"

%}

/* start conditions */

/* regexp nicknames */

%% /* 2. rules */
a((b|a*c)x)*|x*a    { printf("Matched: %s\n", yytext); }

%% /* 3. programs */