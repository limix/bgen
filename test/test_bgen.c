#include "bgen/bgen.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


#define SUCCESS EXIT_SUCCESS
#define FAIL EXIT_FAILURE


int test_filepath(const byte *filepath, VariantIndexing **indexing)
{
    BGenFile *bgen;
    inti e;

    bgen = bgen_open(filepath);

    if (bgen == NULL) return FAIL;

    if (bgen_nsamples(bgen) != 500) return FAIL;

    if (bgen_nvariants(bgen) != 199) return FAIL;

    string *samples = bgen_read_samples(bgen);

    if (samples != NULL)
    {
        e = strncmp("sample_001", (char *)samples[0].str, samples[0].len);

        if (e != 0) return FAIL;

        e = strncmp("sample_500", (char *)samples[499].str, samples[0].len);

        if (e != 0) return FAIL;
    }

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
    VariantGenotype *vg;


    FILE *f = fopen("test/data/example.matrix", "r");

    if (f == NULL) return FAIL;

    char   line[65536];
    double prob[3];
    double eps = 1e-4;
    int    tmp;
    inti   nsamples, ncombs;
    real  *probabilities;

    inti i, j;

    for (i = 0; i < 199; ++i)
    {
        vg = bgen_open_variant_genotype(indexing, i);

        if (bgen_variant_genotype_ploidy(vg) != 2)
        {
            fprintf(stderr, "Wrong ploidy.\n");
            return FAIL;
        }

        nsamples = bgen_variant_genotype_nsamples(vg);
        printf("nsamples: %lld\n", nsamples);

        ncombs = bgen_variant_genotype_ncombs(vg);
        printf("ncombs: %lld\n",   ncombs);

        probabilities = calloc(nsamples * ncombs, sizeof(real));

        bgen_read_variant_genotype(indexing, vg, probabilities);

        for (j = 0; j < 500; ++j)
        {
            tmp = fscanf(f, "%lf", prob + 0);
            tmp = fscanf(f, "%lf", prob + 1);
            tmp = fscanf(f, "%lf", prob + 2);

            // printf("%lf ", probabilities[j * 3 + 0]);
            // printf("%lf ", probabilities[j * 3 + 1]);
            // printf("%lf ", probabilities[j * 3 + 2]);

            // if ((prob[0] == 0) && (prob[1] == 0) && (prob[2] == 0)) prob[2] =
            // 1.0;


            if (fabs(prob[0] - probabilities[j * 3 + 0]) > eps) return FAIL;


            if (fabs(prob[1] - probabilities[j * 3 + 1]) > eps) return
                    FAIL;

            if (fabs(prob[2] - probabilities[j * 3 + 2]) > eps) return
                    FAIL;
        }
        bgen_close_variant_genotype(indexing, vg);
        free(probabilities);
    }

    fclose(f);

    return SUCCESS;
}

int main()
{
    VariantIndexing *indexing;

    // if (test_filepath((byte *)"test/data/example.1bits.bgen", &indexing) ==
    //     FAIL) return FAIL;
    //
    // bgen_free_indexing(indexing);
    //
    // if (test_filepath((byte *)"test/data/example.32bits.bgen", &indexing) ==
    //     FAIL) return FAIL;
    //
    // if (test_probabilities(indexing) == FAIL) return FAIL;
    //
    // bgen_free_indexing(indexing);

    if (test_filepath((byte *)"test/data/example.v11.bgen", &indexing) ==
        FAIL) return FAIL;

    if (test_probabilities(indexing) == FAIL) return FAIL;

    bgen_free_indexing(indexing);

    return EXIT_SUCCESS;
}
