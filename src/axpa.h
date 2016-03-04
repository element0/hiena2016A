/*_HIENA_AXPA_H_*/
#ifndef _HIENA_AXPA_H_
#define _HIENA_AXPA_H_

typedef struct hiena_access_path  Axpa;
typedef struct hiena_parse_packet Ppak;
typedef struct hiena_parse_packet Dpak;
typedef struct hiena_scannerlib   scanlib;

Axpa *new_axpa              ( Axpa * );
void  axpa_refresh          ( Axpa * );
void  axpa_cleanup          ( Axpa * );
void  axpa_set_garbage      ( Axpa *, Axpa * );
int   axpa_set_rql          ( Axpa *, Ppak * );
int   axpa_set_parent_domain( Axpa *, Ppak * );
int   axpa_set_domain       ( Axpa *, Ppak * );
int   axpa_set_scanners     ( Axpa *, scanlib * );
int   axpa_set_scanlib      ( Axpa *, const char * );		/* TODO: rename to axpa_load_scanners */
Ppak *axpa_get_rql          ( Axpa * );
Ppak *axpa_get_parent_domain( Axpa * );
Ppak *axpa_get_domain       ( Axpa * );
scanlib *axpa_get_scanlib   ( Axpa * );			/* TODO: rename to axpa_get_scanners */
void  axpa_print_dir        ( Axpa * );
void  axpa_print_stream     ( Axpa * );



struct hiena_access_path {
    Axpa *next_garbage;
    Axpa *prev_garbage;
    Dpak *rql;
    Dpak *pdomain;
    Dpak *domain;
    scanlib *scanners;
};

#endif /*!_HIENA_AXPA_H_*/
