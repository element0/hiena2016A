#include "domaincell.h"

int dcel_try_lock( dcel *, const char * mode, void * lock_holder ) {
    // check lock
    // there may be another lock on the file from another holder
    // which may be compatible with this mode
    // set lock based on mode and lock_holder
    return 1;
}
int dcel_unlock( dcel *, const char * mode, void * lock_holder ) {
    // unlock based on mode and lock_holder
    // there may still be another lock on the file from another holder
    return 1;
}


struct domaincell_callbacks hiena_domaincell_callbacks {
    .try_lock = dcel_try_lock,
    .unlock   = dcel_unlock,
};
