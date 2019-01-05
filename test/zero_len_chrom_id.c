#include "bgen.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    struct bgen_vi *index;
    struct bgen_file *bgen;
    struct bgen_var *variants;
    struct bgen_str *samples;

    bgen = bgen_open("data/zero_len_chrom_id.bgen");

    if (bgen == NULL)
        return 1;

    if (bgen_nsamples(bgen) != 182)
        return 1;

    if (bgen_nvariants(bgen) != 50)
        return 1;

    samples = bgen_read_samples(bgen, 0);

    if (samples == NULL)
        return 1;

    variants = bgen_read_variants_metadata(bgen, &index, 0);
    if (variants == NULL)
        return 1;

    bgen_free_index(index);
    bgen_close(bgen);

    return 0;
}
