%{
#include <stdio.h>
#include "hiena.h"

int
placeholder_function()
{
    return 100;
}

int
id_child(char *line)
{
    if(line[0]!='	'){
	return 1;
    }else{
    	return 0;
    }
}

int type;
#define CHILD 1

%}

%option noyywrap


%%
^[^ 	\n]+.*[\n]	printf("child:\n%s",yytext); return;
([ 	]+.*[\n])+	printf("childspace:\n%s",yytext); return;

%%

void
ls_children(FILE *infile)
{
    yyin = infile;
    yylex();
}

int
main(int argc, char **argv)
{
    FILE *file = NULL;

    if(argc<2){
	printf("Usage: %s <file>\n",argv[0]);
	return;
    }
    if( (file = fopen(argv[1],"r"))==NULL){
	perror("fopen:");
	return;
    }
    ls_children(file);
    ls_children(file);
    fclose(file);
}
