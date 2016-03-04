%{
#include "cosmosfs-args.tab.h"
#define YY_USER_ACTION yyextra->bytes_read += yyleng;
#define YY_USER_INIT   yylloc->last_column = 0;
%}
%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"

FILEURL		[[:alnum:]._][-:._[:alnum:]]*
CHARSETNAME	[[:print:]]{1,40}
MODULENAME      [[:lower:]]+([_][[:lower:]])*
FUSEOPTNAME	[[:lower:]]+([_][[:lower:]])*
FUSERMOUNTARGS	[-][hVuqz]+
FUSERMOUNTARGO  [-][o]
FUSERMOUNTOPTX	{FUSEOPTNAME}([=]({FILEURL}|{CHARSETNAME}|{MODULENAME}([:]{MODULENAME})*))?
FUSERMOUNTOPTY	{FUSEOPTNAME}[=]({FILEURL}|{CHARSETNAME}|{MODULENAME}([:]{MODULENAME})*)

%s fuseopt
%s fuseoptcont


%%
					
{FUSERMOUNTARGS}			{	yylval->str = strdup(yytext);
						yylloc->first_column = yylloc->last_column+1;
						yylloc->last_column += yyleng;
						return FUSEARG;
					}
{FUSERMOUNTARGO}			{	yylval->str = strdup(yytext);
						yylloc->first_column = yylloc->last_column+1;
						yylloc->last_column += yyleng;
    						yy_push_state(fuseopt, yyscanner);
    						return FUSEARGO;
					}
<fuseopt>{FUSERMOUNTOPTX}		{	yylval->str = strdup(yytext);
						yylloc->first_column = yylloc->last_column+1;
						yylloc->last_column += yyleng;
    						yy_pop_state(yyscanner);
						yy_push_state(fuseoptcont, yyscanner);
    						return FUSEOPT;
					}
{FUSERMOUNTOPTY}			{
						yylloc->first_column = yylloc->last_column+1;
						yylloc->last_column += yyleng;
					}
    		
[/]*{FILEURL}([/]+{FILEURL})*[/]*	{	yylval->str = strdup(yytext);
						yylloc->first_column = yylloc->last_column+1;
						yylloc->last_column += yyleng;
						return FILEPATH;
					}
.					{	
						yylloc->first_column = yylloc->last_column+1;
						yylloc->last_column += yyleng;
    					}
