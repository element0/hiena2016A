/*_HPAT_TEST_*/

#include <stdio.h>
#include "../src/hpat.c"


int main (int argc, char *argv[]) {
    Hpat *h1 = new_hpat();
    Hpat *h2 = new_hpat_from_buf(h1->len, h1->buf);
    Hpat *h3 = new_hpat_from_str("Golden Hour");
    Hpat *h4 = new_hpat_from_uint64((uint64_t)162347124);
    Hpat *h5 = new_hpat_from_argv(argc, argv);
    Hpat *h6 = hpat_dupe(h3);
    char *s  = hpat_str_ptr(h3);
    int   e1 = hpat_is_c_string(h3);
    int   e2 = hpat_cmp(h3,h6);
    FILE *fp = hpat_fopen(h3, "r");

    fclose(fp);

    int   e3 = hpat_cleanup(h1);
    hpat_cleanup(h2);
    hpat_cleanup(h3);
    hpat_cleanup(h4);
    hpat_cleanup(h5);
    hpat_cleanup(h6);

    return 0;
}


