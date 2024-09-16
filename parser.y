%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

// Declare yylex() and yyerror()
int yylex(void);
void yyerror(const char *s);

extern char output_buffer[];
%}

%union {
    int num;
    char *str;
    struct Node *node;
}

%token SELECT FROM WHERE COMMA STAR EQUALS
%token <str> IDENTIFIER
%token <num> NUMBER

%type <node> statement select_statement select_list table_list where_clause condition expression

%%

statement:
    select_statement { print_ast($1, 0); }
    ;

select_statement:
    SELECT select_list FROM table_list where_clause
    {
        $$ = make_select_node($2, $4, $5);
    }
    ;

select_list:
    STAR { $$ = make_star_node(); }
    | select_list COMMA IDENTIFIER
        {
            $$ = append_to_list($1, make_identifier_node($3));
        }
    | IDENTIFIER
        {
            $$ = make_identifier_node($1);
        }
    ;

table_list:
    table_list COMMA IDENTIFIER
        {
            $$ = append_to_list($1, make_identifier_node($3));
        }
    | IDENTIFIER
        {
            $$ = make_identifier_node($1);
        }
    ;

where_clause:
    WHERE condition
        {
            $$ = $2;
        }
    | /* empty */
        {
            $$ = NULL;
        }
    ;

condition:
    expression EQUALS expression
        {
            $$ = make_condition_node($1, $3);
        }
    ;

expression:
    IDENTIFIER
        {
            $$ = make_identifier_node($1);
        }
    | NUMBER
        {
            $$ = make_number_node($1);
        }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}
