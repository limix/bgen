#include "bgen/bgen.h"
#include "cass.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_complex(void);

int main(void)
{
    test_complex();
    return cass_status();
}

void test_complex(void)
{
    size_t i, j, ii, jj;
    const char filename[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;
    int nsamples, nvariants;
    double *probabilities;

    cass_cond((bgen = bgen_file_open(filename)) != NULL);
    cass_cond((nsamples = bgen_file_nsamples(bgen)) == 4);
    cass_cond((nvariants = bgen_file_nvariants(bgen)) == 10);

    struct bgen_str *sample_ids = bgen_read_samples(bgen, 0);

    cass_cond(bgen_str_equal(BGEN_STR("sample_0"), sample_ids[0]));
    cass_cond(bgen_str_equal(BGEN_STR("sample_3"), sample_ids[3]));

    bgen_free_samples(bgen, sample_ids);

    struct bgen_mf *mf = bgen_create_metafile(bgen, "complex.23bits.bgen.metadata", 1, 0);

    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvariants);
    cass_cond(nvariants == 10);

    cass_cond(bgen_str_equal(BGEN_STR("V1"), vm[0].rsid));
    cass_cond(bgen_str_equal(BGEN_STR("M10"), vm[9].rsid));

    int position[] = {1, 2, 3, 4, 5, 7, 7, 8, 9, 10};
    int correct_nalleles[] = {2, 2, 2, 3, 2, 4, 6, 7, 8, 2};
    char *allele_ids[] = {"A",       "G", "A",  "G",   "A",    "G",     "A",
                          "G",       "T", "A",  "G",   "A",    "G",     "GT",
                          "GTT",     "A", "G",  "GT",  "GTT",  "GTTT",  "GTTTT",
                          "A",       "G", "GT", "GTT", "GTTT", "GTTTT", "GTTTTT",
                          "A",       "G", "GT", "GTT", "GTTT", "GTTTT", "GTTTTT",
                          "GTTTTTT", "A", "G"};

    jj = 0;
    for (i = 0; i < (size_t)nvariants; ++i) {
        cass_cond(vm[i].nalleles == correct_nalleles[i]);
        cass_cond(vm[i].position == position[i]);

        for (int j = 0; j < vm[i].nalleles; ++j) {

            cass_cond(bgen_str_equal(BGEN_STR(allele_ids[jj]), vm[i].allele_ids[j]));
            ++jj;
        }
    }

    bgen_free_partition(vm, nvariants);

    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};

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
    for (i = 0; i < (size_t)nvariants; ++i) {
        struct bgen_vg *vg = bgen_open_genotype(bgen, vm[i].vaddr);
        cass_cond(bgen_phased(vg) == phased[i]);

        probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(double));
        bgen_read_genotype(bgen, vg, probabilities);

        double *p = probabilities;
        for (j = 0; j < (size_t)nsamples; ++j) {

            cass_cond(ploidys[jj] == bgen_ploidy(vg, j));
            cass_cond(bgen_missing(vg, j) == 0);

            for (ii = 0; ii < (size_t)bgen_ncombs(vg); ++ii) {
                cass_cond(!(*rp != *p && !(isnan(*rp) && isnan(*p))));
                ++rp;
                ++p;
            }
            ++jj;
        }
        bgen_close_genotype(vg);
        free(probabilities);
    }

    cass_cond(bgen_close_metafile(mf) == 0);
    bgen_file_close(bgen);
}
