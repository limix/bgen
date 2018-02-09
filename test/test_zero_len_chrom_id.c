#include "bgen.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define SUCCESS EXIT_SUCCESS
#define FAIL EXIT_FAILURE

int main() {
    VariantIndexing *index;
    inti nsamples;
    BGenFile *bgen;
    Variant *variants;
    string *samples;
    inti e;

    bgen = bgen_open((byte *)"data/zero_len_chrom_id.bgen");

    if (bgen == NULL)
        return FAIL;

    if (bgen_nsamples(bgen) != 182)
        return FAIL;

    if (bgen_nvariants(bgen) != 50)
        return FAIL;

    samples = bgen_read_samples(bgen);

    if (samples == NULL)
        return FAIL;

    variants = bgen_read_variants(bgen, &index);
    if (variants == NULL)
        return FAIL;

    bgen_free_indexing(index);

    return EXIT_SUCCESS;
}
