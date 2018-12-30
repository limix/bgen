#include "bgen.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    size_t i, j, ii, jj;
    const char filename[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;
    int nsamples, nvariants;
    double *probabilities;

    if ((bgen = bgen_open(filename)) == NULL)
        return 1;

    if ((nsamples = bgen_nsamples(bgen)) != 4)
        return 1;

    if ((nvariants = bgen_nvariants(bgen)) != 10)
        return 1;

    struct bgen_str *sample_ids = bgen_read_samples(bgen, 0);

    if (strncmp("sample_0", sample_ids[0].str, sample_ids[0].len) != 0)
        return 1;

    if (strncmp("sample_3", sample_ids[3].str, sample_ids[3].len) != 0)
        return 1;

    bgen_free_samples(bgen, sample_ids);

    struct bgen_vi *index;
    struct bgen_var *variants = bgen_read_metadata(bgen, &index, 0);

    if (bgen_max_nalleles(index) != 8)
        return 1;

    if (strncmp("V1", variants[0].rsid.str, variants[0].rsid.len) != 0)
        return 1;

    if (strncmp("M10", variants[9].rsid.str, variants[9].rsid.len) != 0)
        return 1;

    int position[] = {1, 2, 3, 4, 5, 7, 7, 8, 9, 10};
    int correct_nalleles[] = {2, 2, 2, 3, 2, 4, 6, 7, 8, 2};
    char *allele_ids[] = {"A",       "G", "A",  "G",   "A",    "G",     "A",
                          "G",       "T", "A",  "G",   "A",    "G",     "GT",
                          "GTT",     "A", "G",  "GT",  "GTT",  "GTTT",  "GTTTT",
                          "A",       "G", "GT", "GTT", "GTTT", "GTTTT", "GTTTTT",
                          "A",       "G", "GT", "GTT", "GTTT", "GTTTT", "GTTTTT",
                          "GTTTTTT", "A", "G"};

    jj = 0;
    for (i = 0; i < nvariants; ++i) {
        if (variants[i].nalleles != correct_nalleles[i])
            return 1;
        if (variants[i].position != position[i])
            return 1;
        for (int j = 0; j < variants[i].nalleles; ++j) {

            if (strncmp(allele_ids[jj], variants[i].allele_ids[j].str,
                        variants[i].allele_ids[j].len) != 0)
                return 1;
            ++jj;
        }
    }

    bgen_free_variants_metadata(bgen, variants);

    bgen_close(bgen);

    struct bgen_vg *vg;

    vg = bgen_open_variant_genotype(index, 0);
    probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(double));
    bgen_read_variant_genotype(index, vg, probabilities);
    bgen_close_variant_genotype(vg);
    free(probabilities);

    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};

    for (i = 0; i < nvariants; ++i) {
        vg = bgen_open_variant_genotype(index, i);
        if (bgen_phased(vg) != phased[i])
            return 1;
        bgen_close_variant_genotype(vg);
    }

    int ploidys[] = {1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 3, 3, 2,
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 4};
    double real_probs[] = {
        1.000000, 0.000000, NAN,      1.000000, 0.000000, 0.000000, 1.000000, 0.000000,
        0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000,
        1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, NAN,      NAN,
        0.000000, 1.000000, 0.000000, 1.000000, 1.000000, 0.000000, 1.000000, 0.000000,
        1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, NAN,
        NAN,      NAN,      1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, NAN,      NAN,
        NAN,      NAN,      1.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000,
        1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000,
        0.000000, 1.000000, NAN,      NAN,      1.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000,
        1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      0.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 1.000000, 0.000000, NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,      NAN,
        NAN,      NAN,      NAN,      NAN,      0.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
        1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 1.000000, 0.000000};

    double *rp = real_probs;

    jj = 0;
    for (i = 0; i < nvariants; ++i) {
        vg = bgen_open_variant_genotype(index, i);

        probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(double));
        double *p = probabilities;
        bgen_read_variant_genotype(index, vg, probabilities);

        for (j = 0; j < nsamples; ++j) {

            if (ploidys[jj] != bgen_ploidy(vg, j))
                return 1;

            if (bgen_missing(vg, j) != 0)
                return 1;

            for (ii = 0; ii < bgen_ncombs(vg); ++ii) {
                if (*rp != *p && !(isnan(*rp) && isnan(*p)))
                    return 1;
                ++rp;
                ++p;
            }
            ++jj;
        }
        bgen_close_variant_genotype(vg);
        free(probabilities);
    }

    bgen_free_index(index);

    return 0;
}
