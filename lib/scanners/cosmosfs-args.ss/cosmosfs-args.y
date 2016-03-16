%code requires {

/*== SYSTEM HEADERS: ==*/
#include <stdio.h>
#include <string.h>
/*--------*/

/*== PROJECT HEADERS: ==*/
/*#include "../../src/payload->source.h"*/
/*--------*/

/*== REPLACE RELIANCE ON ATAB HEADER: ==*/
/* define own prototypes */
#include "../../src/hiena.h"
/*--------*/

/*== FLEX SETUP: ==*/
typedef void* yyscan_t;
/*--------*/
}
%define api.pure full
%locations
%param {yyscan_t *scanner}
%parse-param {Hsp *payload}

%union {
    int   c;
    char* str;
}
%token <str> FILEPATH FUSEARG FUSEARGO FUSEOPT DISCARD
%token <c> NULCHAR

%destructor { free ($$); } <str>

%%



cosmosfs_arguments	:	valid_cosmosfs_args
				{ /* printf("valid cosmosfs arguments.\n"); */
				  return;
				}
			;
valid_cosmosfs_args	:	commandname source fuseargs mountpoint
			|	commandname source mountpoint
			;
commandname		:	FILEPATH
				{   payload->put( payload->source, "commandname", $1,
					@1.first_line, @1.first_column,
					@1.last_line,  @1.last_column);
				    free($1);
				}
			;
source			:	FILEPATH
				{   payload->put( payload->source, "source", $1,
					@1.first_line, @1.first_column,
					@1.last_line,  @1.last_column);
				    free($1);
				}
			|	source FILEPATH
				{   payload->put( payload->source, "source", $2,
					@2.first_line, @2.first_column,
					@2.last_line,  @2.last_column);
				    free($2); }
			;
fuseargs		:	fusearg
			|	fuseargs fusearg
			|	fuseargs fuseopts
			|	fuseopts
			;
fusearg			:	FUSEARG
				{   payload->put( payload->source, "fusearg", $1,
					@1.first_line, @1.first_column,
					@1.last_line,  @1.last_column);
				    free($1); }
			;
fuseopts		:	FUSEARGO FUSEOPT
				{   payload->put( payload->source, "fusearg-o", $1, 
					@1.first_line, @1.first_column,
					@1.last_line,  @1.last_column);
				    free($1);
				    payload->put( payload->source, "fuseopt", $2, 
					@2.first_line, @2.first_column,
					@2.last_line,  @2.last_column);
				    free($2);
				}
			;
mountpoint		:	FILEPATH
				{   payload->put( payload->source, "mountpoint", $1,
					@1.first_line, @1.first_column,
					@1.last_line,  @1.last_column);
				    free($1);
				}
			;



%%

yyerror(const char *s) {
    fprintf(stderr, "argv.ss err: %s\n", s);
}

int
hiena_scannerop_parse(Hsp *payload) {
    yyscan_t scanner;
    yylex_init_extra( payload, &scanner );


    payload->fp = payload->fopen(payload->source, "r");	 //<--- TBD scannerservers

    yyset_in( payload->fp, scanner );

    if(payload==NULL){
	perror("payload is NULL\n");
	return -1;
    }

    yyparse(scanner, payload);

  
    fclose(payload->fp);
    yylex_destroy(scanner);

    return 0;
}


/*
int
OLD_hiena_scannerop_parse(FILE *fp, Hsp *payload) {
    yyscan_t scanner;
    yylex_init_extra( payload, &scanner );

    yyset_in( fp, scanner );

    yyparse(scanner, payload);

   
    yylex_destroy(scanner);

    return 0;
}
*/
/*
int
main() {
    yyparse();
}
*/
