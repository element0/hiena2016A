%{
#include <stdio.h>
#include "dir.tab.h"
%}


FILECHARS	[[:alnum:]_.-]+


%option noyywrap
%%

[/]*{FILECHARS}([/]+{FILECHARS})*[/]*	yylval.sval = yytext; return FILEPATH;
[\n]					return 0;
