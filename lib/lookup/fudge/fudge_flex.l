%{
#include "fudge_bison.h"
%}
%option reentrant
%option stack noyywrap
%option bison-bridge
/* %option bison-locations */
%option extra-type="Hframe *"

%{
    /* FROM flex's "*.yy.c" skeleton re. YY_INPUT:

     * Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
     * is returned in "result". */


#define YY_INPUT(buf,result,max_size)\
        { \
	    result = yyextra->op->read(buf, 1, max_size, yyextra); \
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
