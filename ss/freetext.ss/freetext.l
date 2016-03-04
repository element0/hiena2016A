%{
#include "freetext.tab.h"

%}

%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"

%%

<<EOF>>		return END;
.		return CHAR;

