

/* lookup language.
   executes the lookup pattern sent to the lookup function.

   the lookup function operates according to FUSE design.

   the lookup scannerserver must create and return a "fuse_entry_param" object
   conformant to the design of FUSE.

   fuse_lookup_prototype(fuse_req_t req, fuse_ino_t access_path, const char *lookup_pattern);

   struct fuse_entry_param {
       fuse_ino_t    ino;
       unsigned long generation;
       struct stat   attr;
       double        attr_timeout;
       double        entry_timeout;
   };
   */

/* see also "Hiena/design/snails"
   for a lookup language.
   */
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

%token NONRESERVED
%token SPACE TAB AT DOT COMMA HASH COLON SEMICOLON TILDE EQUALS CARET;
%token OPENBRACKET CLOSEBRACKET
%token FWD_SLASH CR END
/* %token <str> ... */
/* %destructor { free ($$); } <str> */


%%

lookup_pattern	:	ucommand_list END
			{ printf("good.\n"); return; }
		;

ucommand_list	:	ucommand
			{ printf("ucommand, "); }
		|	ucommand_list ucmd_list_sep opt_ws ucommand
			{ printf("separator, ucommand, "); }
		;

ucommand	:	value
		;
/*		|	value at
			{ printf("properties set selector, "); }
		|	assignment
			{ printf("assignment, "); }
		|	comparison
			{ printf("comparison, "); }
		;*/

ucmd_list_sep	:	COMMA
		;


value		:	string_val
			{ printf("string value, "); }
		;
/*
		|	value_range
			{ printf("value range, "); }
		|	list_val
		;
*/

string_val	:	string_nonws
		|	string_nonws string_chars_seq
		;

string_chars_seq:	string_chars
		|	string_chars_seq string_chars
		;

string_chars    :	string_nonws
		|	whitespace
		;

string_nonws	:	dot
		|	NONRESERVED
		;
dot		:	DOT
		;
whitespace	:	SPACE
		|	TAB
		;

opt_ws		:	%empty /* empty */
		|	whitespace_seq
		;

whitespace_seq	:	whitespace
		|	whitespace_seq whitespace
		;
/* MICRO-COMMANDS */
/*
comparison	:	property_key match_op value
			{ printf("property match value, "); }
		;

assignment	:	property_key assign_op value
			{ printf("property assign value, "); }
		;	

property_key    :	at string_nonws
			{ printf("property key, "); }

		;
*/
/*
value_range	:	string_val dot dot string_val
		|	value_range dot dot string_val
		;


list_val	:	OPENBRACKET opt_ws list_val_seq opt_ws CLOSEBRACKET
		;

list_val_seq	:	string_val
			{ printf("string value, "); }
		|	value_range
			{ printf("value range, "); }
		|	list_val_seq opt_ws COMMA opt_ws string_val
			{ printf("string value, "); }
		|	list_val_seq opt_ws COMMA opt_ws value_range
			{ printf("value range, "); }
		;
*/


/* OPERATORS */
/*
assign_op	:	COLON EQUALS
			{ printf("assign_op, "); }
		;
*/
/* TBD: */
/* OTHER ASSIGNMENTS:
   AWK uses c style assignment operators
   '=', '+=', '-=', '*=', '/=', '%=', '^='
   Pascal only uses one.
   ':='
   Reserved shell characters:
   '*' can be escaped
   '/' can't be escaped
   merged ideas:
   ':=', ':+=', ':-=', ':\*=', ':v=', ':%=', ':^='

 */
/*
match_op	:	equals_op
			{ printf("equals_op "); }
		;
*/
/*
		|	regex_match_op
		|	neg_op equals_op
		|	equals_op neg_op
		;
*/
/*
equals_op	:	EQUALS		*//* intuitive to farmer john */
		/*|	EQUALS EQUALS	*//* c style */
		/*;*/
/*
regex_match_op	:	TILDE		// awk style  
		|	EQUALS TILDE	// bash style
		;

neg_op		:	CARET
		;
*/
/*
at		:	AT
		;
*/

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
