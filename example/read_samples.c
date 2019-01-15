/* read_samples.c */
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

    printf("Number of samples: %d\n", bgen_nsamples(bgen));
    printf("Number of variants: %d\n", bgen_nvariants(bgen));

    // Reading sample identifications.
    int verbose = 0;
    struct bgen_str *sample_ids = bgen_read_samples(bgen, verbose);

    printf("First sample ID: %.*s\n", sample_ids[0].len, sample_ids[0].str);
    printf("Second sample ID: %.*s\n", sample_ids[1].len, sample_ids[1].str);

    // Releasing memory associated with sample identifications.
    bgen_free_samples(bgen, sample_ids);

    bgen_close(bgen);

    return 0;
}
