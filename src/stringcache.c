/* string cache */

/* sun feb 07, 2016 RH */

#include "stringcache.h"
#include "hash_hsieh.c"

uint64_t stringcache_string_to_sn ( Stringcache *sc, char *str ) {
    if( sc == NULL || str == NULL ) return 0;

    uint64_t sn = sc->next_sn++;
    kcdbadd( sc->db, str, strlen(str), (const char *)&sn, sizeof(uint64_t) );
    return sn;
}

void stringcache_print ( Stringcache *sc ) {
}

void stringcache_cleanup ( Stringcache *sc ) {
    kcdbclose(sc->db);
    kcdbdel(sc->db);
    free(sc);
}
Stringcache *new_stringcache () {
    Stringcache *sc = malloc(sizeof(*sc));
    sc->db = kcdbnew();
    sc->next_sn = 1;
    if(!kcdbopen(sc->db, ":", KCOWRITER | KCOCREATE )) {
	stringcache_cleanup( sc );
	return NULL;
    }

    return sc;
}


