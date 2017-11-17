#include "bgen/bgen.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

const byte example_1bits[] = "test/data/example.1bits.bgen";
const byte example_32bits[] = "test/data/example.32bits.bgen";
const byte example_v11[] = "test/data/example.v11.bgen";

const byte eg1bits_idx[] = "test/data/example.1bits.bgen.index";
const byte eg32bits_idx[] = "test/data/example.32bits.bgen.index";
const byte egv11_idx[] = "test/data/example.v11.bgen.index";

int test_read_data(BGenFile *bgen, string *samples, Variant *variants) {
    inti e;

    if (bgen_nsamples(bgen) != 500)
        return FAIL;

    if (bgen_nvariants(bgen) != 199)
        return FAIL;

    if (samples != NULL) {
        e = strncmp("sample_001", (char *)samples[0].str, samples[0].len);

        if (e != 0)
            return FAIL;

        e = strncmp("sample_500", (char *)samples[499].str, samples[0].len);

        if (e != 0)
            return FAIL;
    }

    e = strncmp("SNPID_2", (char *)variants[0].id.str, variants[0].id.len);

    if (e != 0)
        return FAIL;

    e = strncmp("SNPID_200", (char *)variants[198].id.str,
                variants[198].id.len);

    if (e != 0)
        return FAIL;

    return SUCCESS;
}

int test_read(const byte *filepath, const byte *index_filepath, VariantIndexing **index) {
    BGenFile *bgen;
    Variant *variants;
    string *samples;

    bgen = bgen_open(filepath);

    if (bgen == NULL)
        return FAIL;

    samples = bgen_read_samples(bgen);
    if (index_filepath)
        variants = bgen_load_variants(bgen, index_filepath, index);
    else
        variants = bgen_read_variants(bgen, index);

    if (test_read_data(bgen, samples, variants) == FAIL)
        return FAIL;

    bgen_free_samples(bgen, samples);
    bgen_free_variants(bgen, variants);

    bgen_close(bgen);

    return SUCCESS;
}

int test_probabilities(VariantIndexing *index, inti nsamples) {
    VariantGenotype *vg;
    FILE *f;
    double prob[3];
    double eps = 1e-4;
    inti ncombs;
    inti i, j;
    real *probabilities;

    f = fopen("test/data/example.matrix", "r");

    if (f == NULL)
        return FAIL;

    for (i = 0; i < 199; ++i) {
        vg = bgen_open_variant_genotype(index, i);

        if (bgen_ploidy(vg) != 2) {
            fprintf(stderr, "Wrong ploidy.\n");
            return FAIL;
        }

        ncombs = bgen_ncombs(vg);

        probabilities = calloc(nsamples * ncombs, sizeof(real));

        bgen_read_variant_genotype(index, vg, probabilities);

        for (j = 0; j < 500; ++j) {
            fscanf(f, "%lf", prob + 0);
            fscanf(f, "%lf", prob + 1);
            fscanf(f, "%lf", prob + 2);

            if ((prob[0] == 0) && (prob[1] == 0) && (prob[2] == 0)) {
                prob[0] = NAN;
                prob[1] = NAN;
                prob[2] = NAN;

                if (!isnan(probabilities[j * 3 + 0]))
                    return FAIL;

                if (!isnan(probabilities[j * 3 + 1]))
                    return FAIL;

                if (!isnan(probabilities[j * 3 + 2]))
                    return FAIL;
            } else {
                if (fabs(prob[0] - probabilities[j * 3 + 0]) > eps)
                    return FAIL;

                if (fabs(prob[1] - probabilities[j * 3 + 1]) > eps)
                    return FAIL;

                if (fabs(prob[2] - probabilities[j * 3 + 2]) > eps)
                    return FAIL;
            }
        }
        bgen_close_variant_genotype(index, vg);
        free(probabilities);
    }

    fclose(f);

    return SUCCESS;
}

int main() {
    VariantIndexing *index;
    inti nsamples;
    nsamples = 500;

    if (test_read((byte *)example_1bits, NULL, &index) == FAIL)
        return FAIL;

    bgen_free_indexing(index);

    if (test_read((byte *)example_32bits, NULL, &index) == FAIL)
        return FAIL;

    if (test_probabilities(index, nsamples) == FAIL)
        return FAIL;

    bgen_free_indexing(index);

    if (test_read((byte *)example_v11, NULL, &index) == FAIL)
        return FAIL;

    if (test_probabilities(index, nsamples) == FAIL)
        return FAIL;

    bgen_free_indexing(index);

    if (test_read((byte *)example_1bits, eg1bits_idx, &index) == FAIL)
        return FAIL;

    bgen_free_indexing(index);

    if (test_read((byte *)example_1bits, eg32bits_idx, &index) == FAIL)
        return FAIL;

    bgen_free_indexing(index);

    if (test_read((byte *)example_1bits, egv11_idx, &index) == FAIL)
        return FAIL;

    bgen_free_indexing(index);

    return SUCCESS;
}
