%code requires {
#include <stdio.h>
#include <string.h>
#include "../../src/scanner.h"	/* HIENA HEADER */
typedef void* yyscan_t;		/* FLEX SETUP */
}
%define lr.type ielr
%define api.pure full
%locations

%param {yyscan_t *scanner}
%parse-param {Hsp *payload}

%union {
    int   c;
    char* str;
}

%token REGEX_MATCH_OP ASSIGNMENT_OP LITERAL_MATCH_OP
%token PROPERTY_NAME INPUT_STRING
%token END
/* %token <str> ... */
/* %destructor { free ($$); } <str> */


%%

ucmd_tester	:	cmd_list END		{ printf("good.\n"); return 0; };

cmd_list	:	cmd;
cmd		:	literal_match		{printf("literal match ");}
		|	regex_match		{printf("regex match ");}
		|	assignment		{printf("assignment ");}
		;
regex_match	:	property_name regex_match_op input_string
		;
literal_match	:	property_name literal_match_op input_string
		|	property_name literal_match_op property_name
		|	property_name
		|	input_string
		;
assignment	:	property_name assignment_op input_string
		|	property_name assignment_op property_name
		;

assignment_op	:	ASSIGNMENT_OP		{printf("assignment op ");};
regex_match_op	:	REGEX_MATCH_OP		{printf("regex match op ");};
literal_match_op:	LITERAL_MATCH_OP	{printf("literal match op ");};
property_name	:	PROPERTY_NAME		{printf("property_name ");};
input_string	:	INPUT_STRING		{printf("input string ");};



%%
yyerror(const char *s) {
    fprintf(stderr, "lookup.ss err: %s\n", s);
}

int
hiena_scannerop_parse(FILE *fp, Hsp *payload) {
    yyscan_t scanner;
    yylex_init_extra( payload, &scanner );

    yyset_in( fp, scanner );
    
    yyparse(scanner, payload);

    yylex_destroy(scanner);

    return 0;
}

/* FOR TESTING */
int
main()
{
    FILE *fp = stdin;
    Hsp  *payload = NULL;
    hiena_scannerop_parse(fp, payload);
}
