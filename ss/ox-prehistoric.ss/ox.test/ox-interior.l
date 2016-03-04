

INDENTCHARS	[ ]
STARTINDENT	^[ ]
ABORTINDENT     ^([^ ]|[\n\v\f])
EOL		[\n\v\f]



%option noyywrap
%{
#include <string.h>

#define TABSPACE 8

int saveindentlen = 0;
int start_countmode = 1;
int shavelevel = 0;


%}

%x countmode
%x shavemode
%x outputmode


%%
	if ( start_countmode ) { BEGIN(countmode); start_countmode = -1; }

<*>{ABORTINDENT}	{ exit(0); }

<countmode>{STARTINDENT}	{ saveindentlen++; }
<countmode>{INDENTCHARS}	{ saveindentlen++; }
<countmode>{EOL}		{ exit(0); }
<countmode>.			{ printf("%s",yytext); BEGIN(outputmode); }

<outputmode>{STARTINDENT}	{ BEGIN(shavemode); shavelevel++; }
<outputmode>.			{ printf("%s",yytext); }

<shavemode>{STARTINDENT}	{ shavelevel++; }
<shavemode>{INDENTCHARS}	{ if(shavelevel < saveindentlen-1) {
    				      		shavelevel++;
				  }else{
				      shavelevel = 0;
				      BEGIN(outputmode);
				  }
				}

%%

int main()
{
	yylex();

	return 0;
}
