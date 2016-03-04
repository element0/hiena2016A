%{
#include "fudge.tab.h"
%}
%option reentrant
%option stack noyywrap
%option bison-bridge
/* %option bison-locations */
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

<<EOF>>			return END;
\n			return END;
[/]			return SLASH;

@		{ 	
    			yylval->str = yytext;
    			return AT;
		}

[.]			return DOT;

[[:alnum:]_]+[[:alnum:]_-]*	{
    			yylval->str = yytext;
			return NAME_SEG;
		}
[{]([^.}\n][^}\n]*)*[}]		{
    			yylval->str = yytext;
			return FLEXIVAL;
		}
[{]			return LBRACE;
[}]			return RBRACE;


:=			return ASSIGN;

=			return EQ;
==			return EQ;
!=			return NOTEQ;
=lt=			return LT;
=gt=			return GT;
=lte=			return LTE;
=gte=			return GTE;
~=			return ERE;
