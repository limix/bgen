#include "bgen/bgen.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


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

int test_probabilities(VariantIndexing *indexing)
{
    VariantGenotype *vg = bgen_read_variant_genotypes(indexing, 0, 199);

    FILE *f = fopen("test/data/example.matrix", "r");

    if (f == NULL) return FAIL;

    char   line[65536];
    double prob[3];
    double eps = 1e-4;

    for (inti i = 0; i < 199; ++i)
    {
        if (vg[i].ploidy != 2) return FAIL;

        for (inti j = 0; j < 500; ++j)
        {
            fscanf(f, "%lf", prob + 0);
            fscanf(f, "%lf", prob + 1);
            fscanf(f, "%lf", prob + 2);

            if ((prob[0] == 0) && (prob[1] == 0) && (prob[2] == 0)) prob[2] = 1.0;


            if (fabs(prob[0] - vg[i].probabilities[j * 3 + 0]) >
                eps) return FAIL;

            if (fabs(prob[1] - vg[i].probabilities[j * 3 + 1]) > eps) return FAIL;

            if (fabs(prob[2] - vg[i].probabilities[j * 3 + 2]) > eps) return FAIL;
        }
    }

    fclose(f);
    bgen_free_variant_genotypes(vg, 199);
    return SUCCESS;
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
    if (test_probabilities(indexing) == FAIL) return FAIL;

    bgen_free_indexing(indexing);

    return EXIT_SUCCESS;
}
