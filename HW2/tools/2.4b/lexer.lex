%{
#include <stdio.h>
#include "parser.h"

/* 
 * 正则表达式说明：a((b|a*c)x)*|x*a
 * 匹配以下模式：
 * 1. 以'a'开头，后跟零个或多个(b或a*c)x序列
 * 2. 或者零个或多个'x'后跟一个'a'
 */
%}

%%
a((b|a*c)x)*|x*a { return PATTERN; }
. { return yytext[0]; }
\n {}
%%