%code requires {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/hiena/scanner_module.h"	/* HIENA HEADER */
    /* includes function macros
       'hiena_new_mapcel', 'hiena_add_mapcel', 'hiena_set_celstr' */

typedef void* yyscan_t;			/* FLEX SETUP */

}

%define lr.type ielr
%define api.pure full
%locations
%param {yyscan_t *scanner}
%parse-param {Hsframe * yyextra}
%token-table			/* get token name via yytname[i] */

%union {
    int    len;
    char * str;
    void * mapcel;
}

%token <mapcel> INDENT
%token <mapcel> LINE INDENTED_LINE
%token BLANKLINE NEWLINE
%token END
%type <mapcel> line indented_line_recursive_area outline outline_block dir_of_outlines
%type <mapcel> dir_of_outline_blocks
%token INDENTED_LINE_RECURSIVE_AREA OUTLINE OUTLINE_BLOCK DIR_OF_OUTLINES
%token DIR_OF_OUTLINE_BLOCKS

/*
%destructor { free ($$); } <str>
%destructor { fprintf(stderr,"bison cleanup: %d\n",$$->key.tokid); ppak_cleanup($$); } <mapcel>
*/

%{
    int num_ox=0;
    int prev_indent=0;
%}

%%
			  /* this file is WIP,
			     an interpretation of ox.def, see ox.def */
valid_ox		: ox
			  {
			      hiena_verbose("ox good");
			      return;
			  }
			;


/* HIERARCHY LEVEL */		
ox			: dir_of_outline_blocks
			  END

			| dir_of_outline_blocks
			  blank_or_indented_lines
			  END

			| blank_or_indented_lines
			  dir_of_outline_blocks
			  END

			| blank_or_indented_lines
			  dir_of_outline_blocks
			  blank_or_indented_lines
			  END
			;

dir_of_outline_blocks	: outline_block
			  {
			      hiena_verbose("DIR_OF_OUTLINE_BLOCKS");

			      hiena_new_mapcel( DIR_OF_OUTLINE_BLOCKS );
			      hiena_add_child( $[outline_block] );
			  }

			| dir_of_outline_blocks
			  blank_or_indented_lines
			  outline_block
			  {
			      hiena_verbose("DIR_OF_OUTLINE_BLOCKS");

			      hiena_add_child( $[outline_block] );
			  }
			;
/* HIERARCHY LEVEL */
outline_block		: dir_of_outlines
			  /* semantic value pass thur */
			  {
			      hiena_verbose("DIR_OF_OUTLINES");
			  }
			;

dir_of_outlines		: outline
			  {
			      hiena_new_mapcel( DIR_OF_OUTLINES );
			      hiena_add_child( $[outline] );
			  }
			| dir_of_outlines outline
			  {
			      hiena_add_child( $[outline] );
			  }
			;

/* HIERARCHY LEVEL */
outline			: LINE
			  {
			      hiena_verbose("OUTLINE");

			      hiena_new_mapcel( OUTLINE );
			      hiena_add_mapcel( $1 );
			  }
			| LINE
			  indented_line_recursive_area
                          {
			      hiena_verbose("OUTLINE");

			      hiena_new_mapcel( OUTLINE );
			      hiena_add_mapcel( $1 );
			      hiena_add_mapcel( $2 );
			  }
			;


indented_line_recursive_area
			: INDENT INDENTED_LINE
			  {
			      hiena_verbose("INDENTED_LINE_RECURSIVE_AREA");

			      hiena_new_mapcel( INDENTED_LINE_RECURSIVE_AREA );
			      hiena_add_mapcel( $1 );
			      hiena_add_mapcel( $2 );
			      /*
			      if (prev_indent>$1)
			          printf("}\n");
			      else if (prev_indent<$1)
				  printf("{\n");
			      printf("%s",$2);
			      prev_indent=$1;
			  */}
			| indented_line_recursive_area INDENT INDENTED_LINE
			  {
			      hiena_verbose("INDENTED_LINE_RECURSIVE_AREA");

			      hiena_add_mapcel( $2 );
			      hiena_add_mapcel( $3 );
			      /*
			      if (prev_indent>$2)
			          printf("}\n");
			      else if (prev_indent<$2)
				  printf("{\n");
			      printf("%s",$3);
			      prev_indent=$2;
			  */}
			;

/****** NON MAPPING SECTION ********/

blank_or_indented_lines	:	BLANKLINE
			|	BLANKLINE blank_or_indented_lines2
			;

blank_or_indented_lines2:	BLANKLINE
			|	INDENT INDENTED_LINE
			|	blank_or_indented_lines2 BLANKLINE
			|	blank_or_indented_lines2 INDENT INDENTED_LINE
			;



			
			

%%
/*
struct hiena_scannerserver sslookup = {
    .allowed_child_scanners = { "ulang" }	// {:: ulang ::} 
};

char **
show_allowed_child_scanners() {
    return sslookup.allowed_child_scanners;
}
*/

const char *
get_tok_name(int tokid) {
    return yytname[tokid-256+1];
    //return "simple sam";
}

/* leave return type blank - bison caveat */
yyerror(char const *s) {
    fprintf(stderr, "ox.ss err: %s\n", s);
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
/* int
main()
{
    FILE *fp = stdin;
    Hsp  *payload = NULL;
    hiena_scannerop_parse(fp, payload);
}
*/
