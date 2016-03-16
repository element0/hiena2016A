/* skel */

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

%token BASENAME FALSE_SUFFIX SUFFIX
%token END
/* %token <str> ... */
/* %destructor { free ($$); } <str> */


%%
filename	:	BASENAME
		|	BASENAME suffix_seq
			{ printf("good.\n"); return; }
		;

suffix_seq	:	SUFFIX
		|	FALSE_SUFFIX
		|	suffix_seq SUFFIX
		|	suffix_seq FALSE_SUFFIX
		;


%%
yyerror(const char *s) {
    fprintf(stderr, ".ss err: %s\n", s);
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
