#ifndef _HIENA_DOMAINCELL_H_
#define _HIENA_DOMAINCELL_H_

/** @file domaincell.h

  Hiena Domain Cell Structure 

  NOTE:  this was previously called 'struct hiena_domainstream'
  but that name was potentially suggesting a POSIX streams behaviour.
  I am renaming to 'struct hiena_domaincell'.

  A domain cell describes a bounded domain of information as it may exist
  on a media.  

  NOTE:  there are several important subsystems which operate on a 'dcel',
      'dcel' server module
      'hiena_gettok' function
      'sql' interpreter subsystem
      'css selector' interpreter subsystem
  
  the 'dcel server' module opens a file handle and performs POSIX streams-like
  behaviour.  the 'hiena_gettok' function performs map lookups from a scanner's
  POV.  the 'sql' interpreter performs sql queries on a dcel as if it were a
  database.  the 'css selector' interpreter performs selections from the dcel
  as if it were a DOM.

  Each one of these subsystems must have access to the dcel code:

  The 'dcel' server shall include "src/domaincell.h" to access the dcel structure.
  The dcel structure shall include a set of dcel function callbacks
  for the purposes of the dcel server.

  The 'hiena_gettok' function shall be provided as callback to scanners.
  Scanners shall call hiena_gettok from the yylex() function or implement yylex
  as a macro which calls hiena_gettok instead.  'hiena_gettok' has access to
  the complete 'dcel' API.

  The 'sql' and 'css selector' interpreter subsystems can be implemented as
  bison-flex parsers compiled directly into the main program.  They can have
  access to the complete 'dcel' API.

 */

#include <gob.h>
#include <hiena/server_module.h>


typedef struct hiena_domaincell dcel;

/** used by the 'dcel' protocol server
 */
struct domaincell_protocol_address {
    dcel * dcel;
    off_t  off;
    off_t  len;
};

struct domaincell_callbacks {
    int   (*trylock) (dcel *, const char * mode, void * lock_holder );
    int   (*unlock)   (dcel *, const char * mode, void * lock_holder );
};

struct hiena_domaincell {
    gob    general_object_chain_header;	/**< access by typcasting domainstream to (gob) */
    int    syncflags;			/**< sync flags:  0 == out of sync, 1 == in sync */
    struct domaincell_callbacks * op;

    addr_t addr;
    /* provides:
	void * serverlib;
	void * serversign;
	size_t serversign_len;
	void * domain_ptr;
	void * protocol_addr;
	size_t protocol_addr_len;
    */
    void (*serversign_cleanup) ( void * );
    void (*addr_cleanup)       ( void * );

    void * lock;	/* TBD */
};


#endif /*!_HIENA_DOMAINCELL_H_*/
