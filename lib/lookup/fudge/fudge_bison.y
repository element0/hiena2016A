%code requires {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/hiena/lookup_module.h"		/* HIENA HEADER */

typedef void* yyscan_t;			/* FLEX SETUP */

/* callback args */
#define pusharg(SQLARG) h->op->add_va(h, SQLARG)
#define sql(SQLSTR)  h->op->sql(h, SQLSTR, strlen(SQLSTR))
}


%define lr.type ielr
%define api.pure full
/* %locations */
/* %define api.value.type union */
%param {yyscan_t *scanner}
%parse-param {Hframe *h}


%union {
    int    c;
    char * str;
}


%token END
%token <str> NAME_SEG AT FLEXIVAL DOT
%token LBRACE RBRACE SLASH
%token ASSIGN
%token EQ NOTEQ LT GT LTE GTE ERE
%type  <str> name_or_at
%type  <str> child_ident value

%destructor { free ($$); } <str>


%%


ulangpath	:	ucmd END
			{
			    //printf("good.\n");
			    return;
			}
		|	ucmd slash ulangpath END
			{ 
			    //printf("good.\n");
			    return;
			}
		;
slash		:	SLASH
			{
			    //printf("set_target( union( \"selection_prop\" ));\n");
			}
		;

ucmd		:	fudge
		;		

fudge		:	child_ident
			{ 
			    // printf("return_targ()\n");
			}
		| 	assignment
    			{
			    //printf("do_op()\n");
			}
		|	conditional
			{
			    //printf("do_op(targ1,source)\n");
			}
		|	dir_property
			{
			    //printf("return_targ()\n");
			}
		;

child_ident	:	id_base
		|	id_base derivatives
		|	DOT
		|	DOT derivatives
		|	DOT DOT
		|	DOT DOT derivatives
			{
			    //printf("setmap(targ.PROP)\n");
			}
		;

id_base		: name_or_at
      {
	
        //printf("setmap(->CHILD)\n");
	//printf("SELECT * FROM \"selection_children\" WHERE \"name\" IS \"%s\";\n",$1);
	pusharg($1);
	sql("SELECT * FROM \"selection_children\" WHERE \"name\" IS \'%s\';");

	//h->op->set_rqmap(h,RQCHILD);
	//printf("set_targ(map,%s)\n",$1);
	//h->op->set_rqtarg(h,$1);
      }
		| DOT NAME_SEG
      {
	//printf("setmap(CHILD)\n");
	//h->op->set_rqmap(h,RQCHILD);
	//printf("set_targ()\n");
	//h->op->set_rqtarg(h,".");
      }
		;
derivatives	: DOT name_or_at
		  {
		 pusharg($2);
	sql("SELECT \"%s\" FROM \"selection_prop\";");

		    //printf("find_targ(map,%s)\n",$2);
		    //h->op->set_rqmap(h,RQPROP);
		    //h->op->set_rqtarg(h,$2);
		  }
		| derivatives DOT name_or_at 
		  { 

	         pusharg($3);                      pusharg($3);  pusharg($3);
	sql("SELECT \"%s\" FROM \"selection_prop\" WHERE \"%s\" IS NOT \'%s\';");

		    //h->op->set_rqmap(h,RQPROP);
		    //printf("find_targ(map,%s)\n",$3);
		    //h->op->set_rqtarg(h,$3);
		  }
		| LBRACE DOT name_or_at RBRACE
		  {
		  }
		| derivatives LBRACE DOT name_or_at RBRACE 
		  {
		  }
		| LBRACE derivatives DOT name_or_at RBRACE
		  {
		  }
		;

name_or_at	: NAME_SEG
		{
		    printf("name segment\n");
		}
		| AT
		;

assignment	: child_ident assign_op value
		;

conditional	: child_ident condition_op value
		;

value		: child_ident
		{
		    //h->op->set_rqsrc(h,$1);
		    printf("set_src(%s)\n",$1);
		}
		|	FLEXIVAL
		{
		    //Ppak *fvp = new_ppak_from_str($1,NULL);
		   
		    //h->op->set_rqsrc(h,fvp);
		    printf("set_src(%s)\n",$1);
		}
			
		|	LBRACE RBRACE
		{
		    //h->op->set_rqsrc(h,NULL);
		    //printf("set_src(%s)\n",NULL);
		}
		;

assign_op	:	ASSIGN
    {
//	set_op(ASSIGN);
	//printf("set_operator(ASSIGN):%d\n",NULL);
//	set_dest(targ);
	//printf("save_targ(targ,%s)\n",NULL);
//	set_targ("working_dir");
	//printf("set_targ(%s)\n",NULL);
    }
		;
condition_op	: EQ
    {
	//printf("condition_op: set_operator(EQ)\n");
	//printf("condition_op: save_targ(targ,targ1)\n");
	//printf("condition_op: reset_targ(working_dir)\n");
    }
		| NOTEQ
    {
	//printf("condition_op: set_operator(NOTEQ)\n");
	//printf("condition_op: save_targ(targ,targ1)\n");
	//printf("condition_op: reset_targ(working_dir)\n");
    }
		| LT
    {
	//printf("condition_op: set_operator(LT)\n");
	//printf("condition_op: save_targ(targ,targ1)\n");
	//printf("condition_op: reset_targ(working_dir)\n");
    }
		| GT
    {
	//printf("condition_op: set_operator(GT)\n");
	//printf("condition_op: save_targ(targ,targ1)\n");
	//printf("condition_op: reset_targ(working_dir)\n");
    }
		| LTE
    {
	//printf("condition_op: set_operator(LTE)\n");
	//printf("condition_op: save_targ(targ,targ1)\n");
	//printf("condition_op: reset_targ(working_dir)\n");
    }
		| GTE 
    {
	//printf("condition_op: set_operator(GTE)\n");
	//printf("condition_op: save_targ(targ,targ1)\n");
	//printf("condition_op: reset_targ(working_dir)\n");
    }
		| ERE
    {
	//printf("condition_op: set_operator(ERE)\n");
	//printf("condition_op: save_targ(targ,targ1)\n");
	//printf("condition_op: reset_targ(working_dir)\n");
    }
		;
dir_property    :	dir_property_seg
		|	dir_property_seg dir_property	
		;
dir_property_seg: LBRACE name_or_at RBRACE
		| LBRACE DOT name_or_at RBRACE
		| LBRACE DOT name_or_at RBRACE derivatives
		| LBRACE name_or_at derivatives RBRACE
		| LBRACE DOT name_or_at derivatives RBRACE
		;


%%




/* leave return type blank - bison caveat */
yyerror(char const *s) {
    fprintf(stderr, "fudge err: %s\n", s);
}
