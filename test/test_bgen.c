#include "bgen/bgen.h"

#include <stdio.h>
#include <string.h>


#define SUCCESS EXIT_SUCCESS
#define FAIL EXIT_FAILURE


int test_filepath(const byte *filepath, VariantIndexing **indexing)
{
    BGenFile *bgen;

    bgen = bgen_open(filepath);

    if (bgen == NULL) return FAIL;

    if (bgen_nsamples(bgen) != 500) return FAIL;

    if (bgen_nvariants(bgen) != 199) return FAIL;

    string *samples = bgen_read_samples(bgen);

    if (samples == NULL) return FAIL;

    inti e = strncmp("sample_001", (char *)samples[0].str, samples[0].len);

    if (e != 0) return FAIL;

    e = strncmp("sample_500", (char *)samples[499].str, samples[0].len);

    if (e != 0) return FAIL;


    Variant *variants = bgen_read_variants(bgen, indexing);

    e = strncmp("SNPID_2", (char *)variants[0].id.str, variants[0].id.len);

    if (e != 0) return FAIL;

    e = strncmp("SNPID_200", (char *)variants[198].id.str, variants[198].id.len);

    if (e != 0) return FAIL;

    bgen_free_samples(bgen, samples);
    bgen_free_variants(bgen, variants);

    bgen_close(bgen);

    return EXIT_SUCCESS;
}

int main()
{
    VariantIndexing *indexing;

    if (test_filepath((byte *)"test/data/example.1bits.bgen", &indexing) ==
        FAIL) return FAIL;

    bgen_free_indexing(indexing);

    if (test_filepath((byte *)"test/data/example.32bits.bgen", &indexing) ==
        FAIL) return FAIL;

    // VariantGenotype *vg = bgen_read_variant_genotypes(index, 0, 2);
    bgen_free_indexing(indexing);

    return EXIT_SUCCESS;
}
