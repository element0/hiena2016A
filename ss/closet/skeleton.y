%code requires {
#include <stdio.h>
#include <string.h>
#include "../../include/hiena.h"	/* HIENA HEADER */

typedef void* yyscan_t;			/* FLEX SETUP */
}

%define lr.type ielr
%define api.pure full
%locations
%param {yyscan_t *scanner}
%parse-param {Hsp *payload}

%token END

%%
skeleton	:	END
			{ printf("good.\n"); return; }
		;


%%
struct hiena_scannerserver sslookup = {
    .allowed_child_scanners = { "ulang" }	/* {:: ulang ::} */
};

char **
show_allowed_child_scanners() {
    return sslookup.allowed_child_scanners;
}

/* leave return type blank - bison caveat */
yyerror(char const *s) {
    fprintf(stderr, "skeleton.ss err: %s\n", s);
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
