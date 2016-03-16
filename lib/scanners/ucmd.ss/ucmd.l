%{
#include "ucmd.tab.h"

%}

%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"

%%
\n		return END;
<<EOF>>		return END;
	/* ucmd */
:=		return ASSIGNMENT_OP;
=~		return REGEX_MATCH_OP;
[=]{1,2}/[^~]	return LITERAL_MATCH_OP;

	/* property name */
[@][[:alpha:]][[:alnum:]_-]*	return PROPERTY_NAME;
	
	/* input string */
[^\n:=~]+			return INPUT_STRING;



