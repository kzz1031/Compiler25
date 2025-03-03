%{
#include <stdio.h>
#include "parser.h"
%}

%%
"("     { printf("LEFT_PAREN\n"); }
")"     { printf("RIGHT_PAREN\n"); }
"["     { printf("LEFT_BRACKET\n"); }
"]"     { printf("RIGHT_BRACKET\n"); }
.       { /* ignore other characters */ }
%%
