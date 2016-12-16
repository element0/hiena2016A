#ifndef _HIENA_DOMAINCELL_HANDLE_H_
#define _HIENA_DOMAINCELL_HANDLE_H_

/** @file domaincell_handle.h

  The hiena domaincell handle is used by:
      scanners to map the domain
      sources (if designed so as) to add children to the domain
      lookup module to request map cells.
 
 */

#include "domaincell.h"



typedef struct hiena_domaincell_handle dcelh;


struct hiena_domaincell_handle {
    dcel  *dcel;
    /* open stream handle */
    void  *fh;
    /* map position */
    void  *prevmappos;
    void  *prevmapcel;
    void  *curmappos;
    void  *curmapcel;
    /* stream position */
    off_t  prevpos;
    off_t  curpos;
    off_t  bytesread;
    /* map callbacks */
    void  *map
    /* stream callbacks */
    void  *serv;
};


#endif /*_HIENA_DOMAINCELL_HANDLE_H_*/
