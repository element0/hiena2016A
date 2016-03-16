%{
#include "filename.tab.h"

%}

%option reentrant stack bison-bridge bison-locations noyywrap
%option extra-type="Hsp *"

%%

\n				return END;
<<EOF>>				return END;
[[:alnum:]][[:alnum:]\ _-]*	return BASENAME;
[.]([[:alnum:]]+)		return SUFFIX;
[.]([[:alnum:]\ _-]+)		return FALSE_SUFFIX;

