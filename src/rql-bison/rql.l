%{
#include "../hiena.h"
#include "rql.tab.h"
%}
%option prefix="rql"
%option reentrant
%option stack noyywrap
%option bison-bridge
/* %option bison-locations */
%option extra-type="Hsp *"

%{
#define YY_INPUT(buf,result,maxsize)\
    { \
	int c = yyextra->op->sql_getchar(yyextra); \
	result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
    } \

%}

%x quot1 quot2

%%

<<EOF>>			return END;

[ \t\n]
(?i:select)		yylval->str = strdup(yytext); return SELECT;
[\*]			yylval->str = strdup(yytext); return STAR;
(?i:all)		yylval->str = strdup(yytext); return ALL;
(?i:from)		yylval->str = strdup(yytext); return FROM;
(?i:where)		yylval->str = strdup(yytext); return WHERE;
(?i:not)		yylval->str = strdup(yytext); return NOT;
(?i:is)			yylval->str = strdup(yytext); return IS;
(?i:like)		yylval->str = strdup(yytext); return LIKE;
[;]			yylval->str = strdup(yytext); return SEMICOLON;
[,]			yylval->str = strdup(yytext); return COMMA;
[']			BEGIN quot1;
<quot1>[^']*		yylval->str = strdup(yytext); return STRING_LITERAL;
<quot1>[']		BEGIN INITIAL;
["]			BEGIN quot2;
<quot2>[^"]*		yylval->str = strdup(yytext); return IDENTIFIER;		
<quot2>["]		BEGIN INITIAL;

