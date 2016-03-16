%code requires {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/hiena.h"	/* HIENA HEADER */

typedef void* yyscan_t;			/* FLEX SETUP */
}

%define lr.type ielr
%define api.pure full
%locations
%param {yyscan_t *scanner}
%parse-param {Hsp *hsp}
%token-table			/* get token name via yytname[i] */

%union {
    int   len;
    char *str;
    Ppak *ob;
}

%token <ob> INDENT
%token <ob> LINE INDENTED_LINE
%token BLANKLINE NEWLINE
%token END
%type <ob> line indented_line_recursive_area outline outline_block dir_of_outlines
%type <ob> dir_of_outline_blocks
%token INDENTED_LINE_RECURSIVE_AREA OUTLINE OUTLINE_BLOCK DIR_OF_OUTLINES
%token DIR_OF_OUTLINE_BLOCKS

%destructor { free ($$); } <str>
%destructor { fprintf(stderr,"bison cleanup: %d\n",$$->key.tokid); ppak_cleanup($$); } <ob>

%{
    int num_ox=0;
    int prev_indent=0;
%}

%%
			  /* this file is WIP,
			     an interpretation of ox.def, see ox.def */
ox_good			: ox
			  {
			      printf("\n");
			      printf("ox good.\n"); return;
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
			      fprintf(stderr, "\n");
			      fprintf(stderr, "DIR_OF_OUTLINE_BLOCKS:\t");
			      fflush(stderr);
			      $$ = hsp->op->make_ob(DIR_OF_OUTLINE_BLOCKS, hsp);
			      hsp->op->add_child($$, $[outline_block], hsp);
			  }
			| dir_of_outline_blocks
			  blank_or_indented_lines
			  outline_block
			  {
			      fprintf(stderr, "\n");
			      fprintf(stderr, "DIR_OF_OUTLINE_BLOCKS:\t");
			      fflush(stderr);
			      hsp->op->add_child($1, $[outline_block], hsp);
			  }
			;
/* HIERARCHY LEVEL */
outline_block		: dir_of_outlines
			  /* semantic value pass thur */
			;

dir_of_outlines		: outline
			  {
			      fprintf(stderr, "\n");
			      fprintf(stderr, "DIR_OF_OUTLINES:\t");
			      fflush (stderr);
			      /* this rule creates the DIR_OF_OUTLINES packet */
			      $$ = hsp->op->make_ob(DIR_OF_OUTLINES, hsp);
			      hsp->op->add_child($$, $[outline], hsp);
			  }
			| dir_of_outlines outline
			  {
			      fprintf(stderr, "\n");
			      fprintf(stderr, "DIR_OF_OUTLINES:\t");
			      fflush (stderr);
			      /* this rule just adds to existing DIR_OF_OUTLINES */
			      hsp->op->add_child($1, $[outline], hsp);
			  }
			;
/* HIERARCHY LEVEL */
outline			: LINE
			  {
			      fprintf(stderr, "\n");
			      fprintf(stderr, "OUTLINE:\t\t");
			      fflush(stderr);
			      $$ = hsp->op->make_ob(OUTLINE, hsp);
			      hsp->op->add_prop($$, $1, hsp);
			  }
			| LINE
			  indented_line_recursive_area
                          {
			      fprintf(stderr, "\n");
			      fprintf(stderr, "OUTLINE:\t\t");
			      fflush(stderr);
			      $$ = hsp->op->make_ob(OUTLINE, hsp);
			      hsp->op->add_prop($$, $1, hsp);
			      hsp->op->add_prop($$, $2, hsp);
			  }
			;


indented_line_recursive_area
			: INDENT INDENTED_LINE
			  {
			      fprintf(stderr, "\n");
			      fprintf(stderr, "INDENTED_LINE_RECURSIVE_AREA:  ");
			      fflush(stderr);
			      $$ = hsp->op->make_ob(INDENTED_LINE_RECURSIVE_AREA, hsp);
			      hsp->op->add_prop($$, $1, hsp);
			      hsp->op->add_prop($$, $2, hsp);
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
			      fprintf(stderr, "\n");
			      fprintf(stderr, "INDENTED_LINE_RECURSIVE_AREA:  ");
			      fflush(stderr);
			      hsp->op->add_prop($1, $2, hsp);
			      hsp->op->add_prop($1, $3, hsp);
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
struct hiena_scannerserver sslookup = {
    .allowed_child_scanners = { "ulang" }	/* {:: ulang ::} */
};

char **
show_allowed_child_scanners() {
    return sslookup.allowed_child_scanners;
}

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
