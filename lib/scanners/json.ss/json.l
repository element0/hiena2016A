%{
#include "json.tab.h"
%}
%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"


%%
<<EOF>>		return END;

	/* Strings */
["]([^"\\]|[\\](["\\/bfnrt]|[u]([0-9]|[a-f]){4}))*["]		return STRINGTOK;
	/*
  ["][^"]+["]	{ printf("STRINGTOK "); return STRINGTOK; }
	    /*
	/* Numbers */
[-]?([0]|[1-9][0-9]*)([.][0-9]+)?([eE][+-]?[0-9]+)?		return NUMBER;

	/* Three literal name tokens: */
true		return TRUETOK;
false		return FALSETOK;
null		return NULLTOK;

	/* JSON Text */
	/* The six structural tokens: */
[[]		return LEFT_SQUARE_BRACKET;
[{]		return LEFT_CURLY_BRACKET;
[]]		return RIGHT_SQUARE_BRACKET;
[}]		return RIGHT_CURLY_BRACKET;
[:]		return COLON;
[,]		return COMMA;

	/* JSON Values (see json.y) */
	/* Objects (see json.y) */
	/* Arrays (see json.y) */
[ \t\n]		/* consume superfluous white space */
