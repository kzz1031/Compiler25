%{
#include <stdio.h>
#include "parser.h"
%}

%%
"("     { return LEFT_PAREN; }
")"     { return RIGHT_PAREN; }
"["     { return LEFT_BRACKET; }
"]"     { return RIGHT_BRACKET; }
.       { /* ignore other characters */ }
%%
