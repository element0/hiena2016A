%{
#include "ulang.tab.h"

%}
%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"

%%
			/* list */
\n			{ return END; }
<<EOF>> 		{ return END; }
([^,\n]|(\x5c,))+	{ return LISTELEMENT; }
[,]/[,]			{ return EMPTYELEMENT; }
[,]

