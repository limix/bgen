#include "bgen.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS EXIT_SUCCESS
#define FAIL EXIT_FAILURE

int main() {
    struct bgen_vi *index;
    struct bgen_file *bgen;
    struct bgen_var *variants;
    struct bgen_str *samples;

    bgen = bgen_open("data/zero_len_chrom_id.bgen");

    if (bgen == NULL)
        return FAIL;

    if (bgen_nsamples(bgen) != 182)
        return FAIL;

    if (bgen_nvariants(bgen) != 50)
        return FAIL;

    samples = bgen_read_samples(bgen, 0);

    if (samples == NULL)
        return FAIL;

    variants = bgen_read_metadata(bgen, &index, 0);
    if (variants == NULL)
        return FAIL;

    bgen_free_index(index);

    return EXIT_SUCCESS;
}
