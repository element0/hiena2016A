%code requires {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hiena.h"

#define rescol(VAR) hsp_sql_set_rescol(h, (void *)VAR)
#define push(HTOK, VAR) rql_push_expr(h, HTOK, (void *)VAR)
#define from(TARG) hsp_sql_set_selection(h, TARG)
#define do_select() hsp_sql_select_core(h)

typedef void *yyscan_t;		/* FLEX SETUP */
}

%define api.prefix {rql};
%define api.pure full
%param {yyscan_t *scanner}
%parse-param {Hsp *h}

%union {
    int   num;
    char *str;
}

%token END EOL
%token <str> SELECT ALL FROM WHERE IS NOT LIKE SEMICOLON COMMA STAR
%token <str> STRING_LITERAL IDENTIFIER

%token EXPR_BINARY_IS EXPR_COLUMN

%token TAB_SRC_CHILD  TAB_SRC_PROP
%token TAB_AXPA_CHILD TAB_AXPA_PROP
%token TAB_SEL_CHILD  TAB_SEL_PROP

%type <str> result_column expr table_or_subquery_list

/* %destructor { free ($$); } <str> */

%%

sql 		: stmt_list END
		{ 
		    return; 
		}
		;


stmt_list	: statement | stmt_list statement
		;


statement	: select_stmt SEMICOLON
		;


select_stmt	: SELECT result_expr_list from_clause where_clause
		{
		    /* at this point the SQL workspace inside the Hsp
		       should be primed.  'do_select()' pulls the trigger. */
		    do_select();
		    printf("select_stmt.\n\n");
		}


		| SELECT result_expr_list from_clause
		{
		    printf("select_stmt.\n\n");
		}
		;


result_expr_list: all_or_distinct result_column_list
		{
		    printf("result_expr_list: all_or_distinct result_column_list\n");
		}
		;


result_column_list
		: result_column
		| result_column_list COMMA result_column
		;


result_column	: STAR
		{ 
		    rescol($1);
		    printf("result_column: %s\n", $1);
		}

		| IDENTIFIER
		{ 
		    rescol($1);  
		    printf("result_column: %s\n", $1);
		}
		;


from_clause	: FROM table_or_subquery_list
		{
		    if(strcmp($2, "selection_children")==0) {
			from(TAB_SEL_CHILD);
		    }else if(strcmp($2, "selection_prop")==0) {
			from(TAB_SEL_PROP);
		    }
		    printf("from_clause: FROM table_or_subquery_list\n");
		}
		;

table_or_subquery_list
		: IDENTIFIER
		{ 
		    printf("table_or_subquery_list: %s\n", $1);
		}
		;

where_clause	: WHERE expr
		{
		    printf("where_clause: WHERE expr\n");
		}
		;

expr		: literal_value
		| binary_expr
		| column_expr
		;

literal_value	: STRING_LITERAL
		{
		    push(STRING_LITERAL, $1);
		}
		;

column_expr	: IDENTIFIER
		{
		    push(EXPR_COLUMN, $1);
		}
		;

binary_expr	: expr IS expr
		{ 
		    push(EXPR_BINARY_IS, NULL);
		    printf("expr: %s IS %s\n", $1, $3);
		}
		| expr IS NOT expr
		{ 
		    printf("expr: %s IS NOT %s\n", $1, $4);
		}
		| expr LIKE expr
		{ 
		    printf("expr: %s LIKE %s\n", $1, $3);
		}
		| expr NOT LIKE expr
		{ 
		    printf("expr: %s NOT LIKE %s\n", $1, $4);
		}
		;

all_or_distinct : %empty
		{ 
		    printf("all_or_distinct: empty\n");
		}
		| ALL
		{ 
		    printf("all_or_distinct: ALL\n");
		}
	/*tbd	| DISTINCT */
		;

%%

/* leave return type blank - bison caveat */
yyerror(char const *s) {
    fprintf(stderr, "rql parser err:\n");
    fflush(stderr);
}

#undef push
#undef rescol
