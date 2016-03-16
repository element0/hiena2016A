/* HIENA_PSERVERS_H */
#ifndef _HIENA_PSERVERS_H_
#define _HIENA_PSERVERS_H_

#include <sys/types.h>	/* off_t, size_t */

typedef struct hiena_server_ops {
    void * (*open)(void * addr, const char *mode)
    void   (*close)(void * );
    size_t (*read)(void * obj, void * ptr, size_t size);
    size_t (*write)(void * obj, void * ptr, size_t size);
}pservops;

void     * pservers_init( const char * path_to_pservers_dir );
pservops * pservers_get( void * pservers, const char * servername );
void       pservers_cleanup( void * pservers );

#endif /*!_HIENA_PSERVERS_H_*/
