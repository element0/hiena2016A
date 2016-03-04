/*_HIENA_SCANNER_H_*/
#ifndef _HIENA_SCANNER_LIB_H_
#define _HIENA_SCANNER_LIB_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>	/* struct dirent */

typedef struct hiena_scannerserver scannerserver;
typedef struct hiena_scannerlib scanlib;

scanlib       *scanlib_create        ();
void           scanlib_cleanup       ( scanlib * );
void           scanlib_print         ( scanlib * );
scannerserver *get_scanner( const char *, scanlib * );	   /* TODO: rename? scanlib_load_scanner() */
scannerserver *slib_get_scanner      ( scanlib *, const char * );
scannerserver *slib_get_next_scanner ( scanlib *, scannerserver * );
scannerserver *scanlib_new_scanner   ( scanlib * );
int            scanlib_rm_scanner    ( scanlib *, scannerserver * );
int            scanlib_scandir_filter( const struct dirent * );
scanlib       *load_scanlib_from_dir ( char *, scanlib * );
scanlib       *load_scanlib_from_path( const char * );	/* PATH style */

#endif /*!_HIENA_SCANNER_LIB_H_*/
