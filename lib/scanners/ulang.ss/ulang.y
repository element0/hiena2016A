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
%token LISTELEMENT EMPTYELEMENT ESC_COMMA END
%%

ulang	:	ucmd_list END
		{ printf("good.\n"); return; }
	;

ucmd_list	:	LISTELEMENT
			{ printf("listelement "); }
		|	ucmd_list LISTELEMENT
			{ printf("listelement "); }
		|	ucmd_list ESC_COMMA LISTELEMENT
			{ printf("esc_comma listelement "); }

		|	EMPTYELEMENT
			{ printf("emptyelement "); }
		|	ucmd_list EMPTYELEMENT
			{ printf("emptyelement "); }
		;

%%

yyerror(const char *s) {
    fprintf(stderr, "ulang.ss err: %s\n", s);
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
