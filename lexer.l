%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>

void set_input_string(const char *str);

%}

%option noyywrap

%%

"SELECT"        { return SELECT; }
"FROM"          { return FROM; }
"WHERE"         { return WHERE; }
","             { return COMMA; }
"*"             { return STAR; }
"="             { return EQUALS; }
[0-9]+          { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return IDENTIFIER; }
[ \t\n]+        { /* skip whitespace */ }
.               { /* ignore other characters */ }

%%

void set_input_string(const char *str) {
    YY_BUFFER_STATE bp = yy_scan_string(str);
    yy_switch_to_buffer(bp);
}
