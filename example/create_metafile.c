/* create_metafile.c */
#include "bgen.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    struct bgen_file *bgen = bgen_open("example.bgen");
    if (bgen == NULL) {
        printf("Failed to open it.\n");
        return 1;
    }
    int npartitions = 2;
    int verbose = 0;
    struct bgen_mf *mf =
        bgen_create_metafile(bgen, "example.bgen.metafile", npartitions, verbose);

    if (bgen_close_metafile(mf)) {
        printf("Failed to close it.\n");
        return 1;
    }

    bgen_close(bgen);

    printf("Finished successfully.\n");

    return 0;
}
