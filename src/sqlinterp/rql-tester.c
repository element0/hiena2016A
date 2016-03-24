#define RQL_TESTER_C

#include <stdio.h>
#include "rql.h"

int main()
{
    yyscan_t scanner;
    rqllex_init_extra( NULL, &scanner );
    do{
	rqlparse(scanner, NULL);
    }while(0);
    rqllex_destroy(scanner);

}
