/* HIENA_DPAKDEX_H */

#ifndef _HIENA_DPAKDEX_H_
#define _HIENA_DPAKDEX_H_

#include <kclangc.h>		/* Kyoto Cabinet */
#include <fuse_low_level.h>
#include "dpak.h"

typedef fuse_ino_t dpakno_t;	/* UNSIGNED. if you change it, rewrite the overflow logic. */


typedef struct hiena_dpak_index {
    dpakno_t next_dpakno;
    Dpak *dpakroot;
    KCDB *ind;
}Dpakdex;


#endif /*!_HIENA_DPAKDEX_H_*/
