%{
#include <stdio.h>
#include <sys/stat.h>

struct stat  *stat_val;

%}

%union {
    int   ival;
    float fval;
    char *sval;
    char  cval;
    struct stat *sbuf;
    mode_t st_mode;
}


%token <sval> FILEPATH


%%
fs	:	file	{ printf("isfile\n"); }
	|	dir	{ printf("isdir\n");  }
	;
file    :	FILEPATH stat isfile
	;
dir	:	FILEPATH stat isdir
	;
stat	:	{
		    
		    if( stat($<sval>0, stat_val) != 0 )
			YYABORT;
		    else
			$<sbuf>stat = stat_val;
		}
	;
isdir	:	{
		    if (!S_ISDIR($<sbuf>0->st_mode))
			YYABORT;
		}
	;
isfile	:	{
		    if (!S_ISREG($<sbuf>0->st_mode))
			YYABORT;
		}
	;
%%

yyerror(const char *s)
{
    fprintf(stderr,"parse err: %s\n",s);
}

int
main()
{
    stat_val = malloc(sizeof(struct stat));
    yyparse();
    free(stat_val);
}
