%{
#include <stdio.h>
#include "parser.h"
%}

%%
(cat)|(cats)|(car)|(cars) { return PATTERN; }
. { return yytext[0]; }
\n {}
%%