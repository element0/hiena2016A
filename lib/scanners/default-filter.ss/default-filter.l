%{
#include "default-filter.tab.h"
%}
%option reentrant
%option stack noyywrap
%option bison-bridge
/* bison-locations */ 
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
<<EOF>>		return END;

