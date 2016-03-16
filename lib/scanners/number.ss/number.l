%{
#include "number.tab.h"
%}
%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"

%%


\n		return END;
<<EOF>>		return END;

[[:digit:]]+([,][[:digit:]]+)*/[^.]			return INTEGER;
[[:digit:]]+([,][[:digit:]]+)*[.]([[:digit:]]+)		return FLOAT;

