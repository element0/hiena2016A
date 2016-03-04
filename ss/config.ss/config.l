/* config file scannerserver definition -- lex file */
/* author: raygan
   date: 3/22/2015
   */

%{
#include <stdio.h>
#include "config.tab.h"
%}


ALPHAS	[[:alpha:]]+
BLANKS  [ \t]+
EOL	[\n\r\f]
ASSIGN  [=]
NUMBER	[0-9]+
POINT   [.]
DQUOT   ["]
SQUOT   [']


%option noyywrap
%%

{ALPHAS}	{ printf("alphas ");	return ALPHAS; }
{BLANKS}	{ printf("blanks ");	return BLANKS; }
{EOL}		{ printf("eol\n");	return EOL; }
{ASSIGN}	{ printf("assign ");	return ASSIGN; }
{NUMBER}	{ printf("number ");	return NUMBER; }
{POINT}		{ printf("point ");	return POINT; }
{DQUOT}		{ printf("dquot ");	return DQUOT; }
{SQUOT}		{ printf("sqout ");	return SQUOT; }

%%

