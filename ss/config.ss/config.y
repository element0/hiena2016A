/* config file scannerserver definition -- yacc/bison style */
/* author: raygan */
/* date: 3/22/2015 */

%{
#include <stdio.h>
%}

%token    ALPHAS BLANKS EOL ASSIGN NUMBER POINT DQUOT SQUOT

%%

config  : varlist					{ printf("config file done\n"); }
        ;
varlist : varlist varline				{ printf("varlist working\n"); }
        | varline
	;
varline : varpair
	| EOL
	;
varpair : ws name ws ASSIGN ws value ws EOL		{ printf("varpair\n"); }
        ;
name	: namestart
	| namestart namepiece
	;
value	: dquoted
	| squoted
	| text
	;
dquoted : DQUOT ws text DQUOT
	;
squoted : SQUOT ws text SQUOT
	;
text	: text textbit
	| textbit
	;
textbit : ALPHAS
	| NUMBER
	| POINT
	| BLANKS
	;
namestart : ALPHAS
	;
namepiece : namepiece namebit
	| namebit
	;
namebit	: ALPHAS
	| NUMBER
	;
ws	: /* empty */
	| BLANKS
	;

	     

%%
int
main(int argc, char *argv[])
{
    yyparse();
}


yyerror(){
    printf("yyerror\n");
}
