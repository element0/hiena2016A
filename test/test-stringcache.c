/* test stringcache */

/* sun feb 07, 2016 RH */

#include <stdio.h>
#include "../src/stringcache.c"

int main ( int argc, char *argv[] ) {
    uint64_t sn;
    Stringcache *stringcache = new_stringcache();

    if ( argc < 2 || stringcache == NULL ) return -1;

    while( argc > 1 ) {

	sn = stringcache_string_to_sn( stringcache, argv[argc-1] );

	printf("%s == %lu\n", argv[argc-1], sn);

	argc--;
    }

    stringcache_print( stringcache );

    size_t sp;
    char *retval = kcdbget(stringcache->db, argv[1], strlen(argv[1]), &sp);
    printf("kcdbget: for %s, %lu\n", argv[1], (uint64_t)retval[0]);
    kcfree(retval);

    stringcache_cleanup( stringcache );

    return 0;
}
