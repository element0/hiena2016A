%{
#include "lookup.tab.h"

%}

%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"

%%

[ ]			{	return SPACE;	}
[\t]			{	return TAB;	}
[@]			{	return AT;      }
[.]			{	return DOT;     }
[,]			{	return COMMA;   }
[#]			{	return HASH;	}
[:]			{ 	return COLON;	}
[;]			{	return SEMICOLON; }
[~]			{	return TILDE;	} 
[=]			{	return EQUALS;	}
\^			{	return CARET;	}
[[]			{	return OPENBRACKET; }
[]]			{	return CLOSEBRACKET;}


	/* normally kernel VFS strips this out of a file path in fs/namei.c */
[/]			{	return FWD_SLASH;	}

	/* end of input signal */
[\n]			{       return END;		}
<<EOF>>			{ 	return END;		}

	/* the meat of a filename */
	/*
	[^@.,\n=:~<>!$^#/]+	{	return WORDCHARS;	}
	*/
.			{	return NONRESERVED;	}


