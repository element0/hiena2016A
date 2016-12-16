#include <stdio.h>
#include "../src/scanlib.c"

int main ( int argc, char * argv[] ) {
    if( argc < 2 ) {
	printf("please specify a path to a scanners directory.\n");
	return -1;
    }

    scanlib * slib = scanlib_load_from_path ( argv[1] );
    printf("loaded scanlib.\n");
    scanlib_print( slib );
    scanlib_cleanup( slib );
    printf("closed scanlib.\n");

    return 0;
}
