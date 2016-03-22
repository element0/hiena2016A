/* HIENA_AXPADEX_H */

#ifndef _HIENA_AXPADEX_H_
#define _HIENA_AXPADEX_H_

#include <kclangc.h>		/* Kyoto Cabinet */
#include <fuse_lowlevel.h>
#include "axpa.h"

typedef fuse_ino_t axno_t;	/* UNSIGNED. if you change it, rewrite the overflow logic */


typedef struct hiena_access_path_index {
    axno_t next_axno;
    Axpa *axroot;
    KCDB *ind;
}Axpadex;

Axpadex *new_axpadex();
axno_t axpadex_get_new_axno( Axpadex * );



#endif /*!_HIENA_AXPADEX_H_*/
