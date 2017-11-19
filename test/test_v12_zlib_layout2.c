#include "example_files.h"

#include <math.h>
#include <stdio.h>
#include <string.h>


int test_read_data(struct BGenFile *bgen, string *s, struct BGVar *v) {
    if (bgen_nsamples(bgen) != 500)
        return FAIL;

    if (bgen_nvariants(bgen) != 199)
        return FAIL;

    if (strncmp("sample_001", s[0].str, s[0].len) != 0)
        return FAIL;

    if (strncmp("sample_500", s[499].str, s[0].len) != 0)
        return FAIL;

    if (strncmp("SNPID_2", v[0].id.str, v[0].id.len) != 0)
        return FAIL;

    if (strncmp("SNPID_200", v[198].id.str, v[198].id.len) != 0)
        return FAIL;

    return SUCCESS;
}

int test_read(const byte *fp0, const byte *fp1, struct BGenVI **index) {
    struct BGenFile *bgen;
    struct BGVar *v;
    string *s;

    bgen = bgen_open(fp0);

    if (bgen == NULL)
        return FAIL;

    s = bgen_read_samples(bgen);
    if (fp1)
        v = bgen_load_variants(bgen, fp1, index);
    else
        v = bgen_read_variants(bgen, index);

    if (test_read_data(bgen, s, v) == FAIL)
        return FAIL;

    bgen_free_samples(bgen, s);
    bgen_free_variants(bgen, v);

    bgen_close(bgen);

    return SUCCESS;
}

int test_probabilities(struct BGenVI *index, inti nsamples, inti prec) {
    struct BGenVG *vg;
    FILE *f;
    double prob[3];
    double eps = (double) (1 << prec);
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
    struct BGenVI *index;
    inti i, prec;
    const byte *ex, *ix;

    for (i = 0; i < get_nexamples(); ++i) {
        ex = get_example(i);
        ix = get_example_index(i);
        prec = get_example_precision(i);

        if (test_read(ex, NULL, &index) == FAIL)
            return FAIL;

        if (test_probabilities(index, 500, prec) == FAIL)
            return FAIL;

        bgen_free_index(index);

        if (test_read(ex, ix, &index) == FAIL)
            return FAIL;

        if (test_probabilities(index, 500, prec) == FAIL)
            return FAIL;

        bgen_free_index(index);
    }

    return SUCCESS;
}
