/* HIENA_AXPADEX_H */

#ifndef _HIENA_ACCESSPATHDB_H_
#define _HIENA_ACCESSPATHDB_H_

#include <kclangc.h>		/* Kyoto Cabinet */
#include <fuse_lowlevel.h>
#include "accesspath.h"

typedef fuse_ino_t axno_t;	/* UNSIGNED. if you change it, rewrite the overflow logic */


typedef struct hiena_access_path_db {
    axno_t next_axno;
    Axpa * axroot;
    KCDB * ind;
}Axpadb;

#endif /*!_HIENA_ACCESSPATHDB_H_*/
