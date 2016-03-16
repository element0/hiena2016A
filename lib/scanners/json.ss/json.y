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

%union {
    char *str;
    int num;
}

%token END
%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET
%token COLON COMMA TRUETOK FALSETOK NULLTOK
%token <num>NUMBER <str>STRINGTOK

%%
json		:	value END
			{ printf("json good.\n"); return; }
		;
value		:	string
		|	NUMBER
			{ printf("number "); }
   		|	array
			{ printf("array "); }
		|	object
			{ printf("object "); }
		|	TRUETOK
			{ printf("TRUETOK "); }
		|	FALSETOK
			{ printf("FALSETOK "); }
		|	NULLTOK
			{ printf("NULLTOK "); }
		;
object		:	LEFT_CURLY_BRACKET object_part_seq RIGHT_CURLY_BRACKET
			{ printf("objecty "); }
		;
object_part_seq	:	string COLON value
			{ printf("keyval pair "); }
		|	object_part_seq COMMA string COLON value
			{ printf(", keyval pair "); }
		;
array		:	LEFT_SQUARE_BRACKET array_part_seq RIGHT_SQUARE_BRACKET;
array_part_seq	:	value
		|	array_part_seq COMMA value
		;
string		:	STRINGTOK
			{printf("string ");}
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
    fprintf(stderr, "json.ss err: %s\n", s);
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
