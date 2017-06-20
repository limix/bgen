#include "bgen/bgen.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


#define SUCCESS EXIT_SUCCESS
#define FAIL EXIT_FAILURE


int test_filepath(const byte *filepath, VariantIndexing **indexing)
{
    // printf("Ponto 1\n");
    BGenFile *bgen;
    inti e;

    bgen = bgen_open(filepath);

    if (bgen == NULL) return FAIL;

    if (bgen_nsamples(bgen) != 500) return FAIL;

    if (bgen_nvariants(bgen) != 199) return FAIL;

    // printf("Ponto 2\n");

    string *samples = bgen_read_samples(bgen);

    // printf("Ponto 2.1\n");

    if (samples != NULL)
    {
        // printf("Ponto 2.2\n");

        e = strncmp("sample_001", (char *)samples[0].str, samples[0].len);

        // printf("Ponto 2.3\n");

        if (e != 0) return FAIL;

        // printf("Ponto 2.4\n");

        e = strncmp("sample_500", (char *)samples[499].str, samples[0].len);

        // printf("Ponto 2.5\n");

        if (e != 0) return FAIL;
    }

    // printf("Ponto 3\n"); fflush(stdout);
    Variant *variants = bgen_read_variants(bgen, indexing);

    // printf("Ponto 3.1\n"); fflush(stdout);

    e = strncmp("SNPID_2", (char *)variants[0].id.str, variants[0].id.len);

    // printf("Ponto 3.2\n"); fflush(stdout);

    if (e != 0) return FAIL;

    // printf("Ponto 3.3\n"); fflush(stdout);

    e = strncmp("SNPID_200", (char *)variants[198].id.str, variants[198].id.len);

    // printf("Ponto 3.4\n"); fflush(stdout);

    if (e != 0) return FAIL;

    // printf("Ponto 4\n"); fflush(stdout);

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
    real  *probabilities;

    inti i, j;

    for (i = 0; i < 199; ++i)
    {
        vg = bgen_open_variant_genotype(indexing, i);

        if (bgen_variant_genotype_ploidy(vg) != 2) return FAIL;

        probabilities = calloc(bgen_variant_genotype_nsamples(
                                   vg) * bgen_variant_genotype_ncombs(
                                   vg), sizeof(real));

        bgen_read_variant_genotype(indexing, vg, probabilities);

        for (j = 0; j < 500; ++j)
        {
            tmp = fscanf(f, "%lf", prob + 0);
            tmp = fscanf(f, "%lf", prob + 1);
            tmp = fscanf(f, "%lf", prob + 2);


            if ((prob[0] == 0) && (prob[1] == 0) && (prob[2] == 0)) prob[2] = 1.0;


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

    if (test_filepath((byte *)"test/data/example.1bits.bgen", &indexing) ==
        FAIL) return FAIL;

    bgen_free_indexing(indexing);

    if (test_filepath((byte *)"test/data/example.32bits.bgen", &indexing) ==
        FAIL) return FAIL;

    if (test_probabilities(indexing) == FAIL) return FAIL;

    bgen_free_indexing(indexing);

    if (test_filepath((byte *)"test/data/example.v11.bgen", &indexing) ==
        FAIL) return FAIL;

    return EXIT_SUCCESS;
}
