/*_HIENA_HPAT_H_*/
#ifndef _HIENA_HPAT_H_
#define _HIENA_HPAT_H_

#include <stdio.h>	/* FILE */
#include <stdint.h>	/* uint64_t */
#include <sys/types.h>  /* off_t */

typedef struct hiena_pattern_buffer Hpat;

Hpat *new_hpat            ();
Hpat *new_hpat_from_buf   ( off_t, void * );
Hpat *new_hpat_from_str   ( const char * );
Hpat *new_hpat_from_uint64( uint64_t );
Hpat *new_hpat_from_argv  ( int, char *argv[] );

Hpat *hpat_from_str       ( const char * );
Hpat *hpat_from_uint64    ( uint64_t );
Hpat *hpat_from_argv      ( int, char *argv[] );

char *hpat_str_ptr        ( Hpat * );
Hpat *hpat_dupe           ( Hpat * );
int   hpat_is_c_string    ( Hpat * );
int   hpat_cmp            ( Hpat *, Hpat * );
FILE *hpat_fopen          ( Hpat *, char * );
int   hpat_cleanup        ( Hpat * );



struct hiena_pattern_buffer {
    void  *buf;
    off_t  len;
};


#endif /*!_HIENA_HPAT_H_*/
