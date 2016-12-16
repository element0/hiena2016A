#ifndef STRINGCACHE_H
#define STRINGCACHE_H

/* string cache header */
/* sun feb 07, 2016 RH */

#include <stdlib.h>
#include <stdint.h>
#include <kclangc.h>	/* kyoto cabinet */
#include <string.h>

typedef struct stringcache {
    uint64_t next_sn;
    KCDB *db;		/* kyoto cabinet polymorphic database */
}Stringcache;

uint64_t stringcache_string_to_sn( Stringcache *, char * );
void    stringcache_print( Stringcache * );
void    stringcache_cleanup( Stringcache * );
Stringcache *new_stringcache();

#endif /* ! STRINGCACHE_H */
