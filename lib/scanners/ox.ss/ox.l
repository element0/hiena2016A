%{
#include <stdlib.h>
#include "ox.tab.h"
#define TAB_TO_SPACE 8
#define YY_USER_ACTION yyextra->cur_location = yyextra->bytes_read;\
    yyextra->bytes_read += yyleng;

static int get_indent_len(char *indent, int tabwidth) {
    int count = 0;
    char c;
    while ((c=indent[0])!='\0'){
	if (c == ' ')
	    count++;
	else if (c == '\t')
	    count+=tabwidth;
	indent++;
    }

    return count;
}

%}
%option reentrant stack bison-bridge noyywrap
%option bison-locations
%option extra-type="Hsp *"
%{

    /* FROM flex's "*.yy.c" skeleton re. YY_INPUT:

     * Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
     * is returned in "result". */

#define YY_INPUT(buf,result,max_size)\
	errno=0; \
	while( (result = yyextra->op->dsread(buf, 1, max_size, yyextra))==0 && \
		yyextra->op->ferror(yyextra) ) \
	{ \
		if( errno != EINTR) \
		{ \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
			break; \
		} \
		errno=0; \
		yyextra->op->clearerr(yyextra); \
	} \

%}


%%
%{
int tab_to_space = TAB_TO_SPACE; 
char *ttsa = yyextra->op->getenv("TAB_TO_SPACE");
int ttsi = 0;

if (ttsa != NULL) {
    ttsi = atoi(ttsa);
}

if (ttsi != 0) {
    tab_to_space = ttsi;
}
//fprintf(stderr, "\nFLEX LOOP:  tab_to_space: %d\n", tab_to_space);
//fflush(stderr);
fprintf(stderr, "\n");
fflush(stderr);

/**** LOCAL VARS ****/
void *b;  /* bounds */

%}

<<EOF>>				{
    printf("EOF\n");
    return END;
				}

^[^ \t\n][^\n]*[\n]		{ 	

    //printf("\"%s\"\n",yytext);
    fprintf(stderr, "\n");
    fprintf(stderr, "LINE:\t");
    fflush(stderr);
    yylval->ob = yyextra->op->make_ob(LINE, yyextra);
    yyextra->op->set_bounds(yylval->ob, 'x', yyextra->cur_location, yyleng);
    yyextra->op->set_buf(yylval->ob, yyleng, yytext);
    return LINE;
				}


^[ \t]+				{

    //printf("\"%s\"\n",yytext);
    fprintf(stderr, "\n");
    fprintf(stderr, "INDENT:\t");
    fflush(stderr);
    yylval->ob = yyextra->op->make_ob(INDENT, yyextra);
    yyextra->op->set_bounds(yylval->ob, 'x', yyextra->cur_location, yyleng);
    yyextra->op->set_buf(yylval->ob, yyleng, yytext);
    fprintf(stderr, "indent len: %ld", get_indent_len(yytext, tab_to_space));
    fflush(stderr);
    return INDENT;
    				}

[^ \t\n][^\n]*[\n]		{	

    //printf("\"%s\"\n",yytext);
    fprintf(stderr, "\n");
    fprintf(stderr, "INDENTED_LINE:\t");
    fflush(stderr);
    yylval->ob = yyextra->op->make_ob(INDENTED_LINE, yyextra);
    yyextra->op->set_bounds(yylval->ob, 'x', yyextra->cur_location, yyleng);
    yyextra->op->set_buf(yylval->ob, yyleng, yytext);
    return INDENTED_LINE;
				}

^([ \t]{-}[\n])*[\n]		{	

    //printf("\"%s\"\n",yytext);
    fprintf(stderr, "\n");
    fprintf(stderr, "BLANKLINE:\t");
    fflush(stderr);
    yylval->ob = yyextra->op->make_ob(BLANKLINE, yyextra);
    yyextra->op->set_bounds(yylval->ob, 'x', yyextra->cur_location, yyleng);
    yyextra->op->set_buf(yylval->ob, yyleng, yytext);
    return BLANKLINE;
				}

.				{
    printf("ignored text\n");
    printf("\"%s\"\n",yytext);
    printf("ignoring.\n");
				}

%%

