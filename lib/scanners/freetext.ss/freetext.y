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

%token END CHAR
/* %token <str> ... */
/* %destructor { free ($$); } <str> */


%%
freetext	:	char_stream END
			{ printf("good.\n"); return; }
		;

char_stream	:	CHAR
		|	char_stream CHAR
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
