#include "bgen.h"
#include "cass.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_file()
{
    size_t i, jj;
    const char filename[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;
    int nsamples, nvariants;

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
    struct bgen_var *variants = bgen_read_variants_metadata(bgen, &index, 0);

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
    for (i = 0; i < (size_t)nvariants; ++i) {
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
    bgen_free_index(index);
    bgen_close(bgen);

    return 0;
}

int test_geno()
{
    const char filename[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;

    if ((bgen = bgen_open(filename)) == NULL)
        return 1;

    struct bgen_mf *mf =
        bgen_create_metafile(bgen, "complex.23bits.bgen.og.metafile", 3, 0);

    assert_equal_int(bgen_metafile_npartitions(mf), 3);
    assert_equal_int(bgen_metafile_nvariants(mf), 10);

    int nvars;
    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvars);

    struct bgen_vg *vg = bgen_open_genotype(bgen, vm[0].vaddr);

    assert_equal_int(bgen_nalleles(vg), 2);
    assert_equal_int(bgen_missing(vg, 0), 0);
    assert_equal_int(bgen_missing(vg, 1), 0);
    assert_equal_int(bgen_missing(vg, 2), 0);
    assert_equal_int(bgen_ploidy(vg, 0), 1);
    assert_equal_int(bgen_ploidy(vg, 1), 2);
    assert_equal_int(bgen_ploidy(vg, 2), 2);
    assert_equal_int(bgen_min_ploidy(vg), 1);
    assert_equal_int(bgen_max_ploidy(vg), 2);
    assert_equal_int(bgen_ncombs(vg), 3);
    assert_equal_int(bgen_phased(vg), 0);

    bgen_close_genotype(vg);

    vg = bgen_open_genotype(bgen, vm[1].vaddr);

    assert_equal_int(bgen_nalleles(vg), 2);
    assert_equal_int(bgen_missing(vg, 0), 0);
    assert_equal_int(bgen_missing(vg, 1), 0);
    assert_equal_int(bgen_missing(vg, 2), 0);
    assert_equal_int(bgen_ploidy(vg, 0), 1);
    assert_equal_int(bgen_ploidy(vg, 1), 1);
    assert_equal_int(bgen_ploidy(vg, 2), 1);
    assert_equal_int(bgen_min_ploidy(vg), 1);
    assert_equal_int(bgen_max_ploidy(vg), 1);
    assert_equal_int(bgen_ncombs(vg), 2);
    assert_equal_int(bgen_phased(vg), 1);

    bgen_close_genotype(vg);

    bgen_free_partition(vm, nvars);

    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};

    size_t i = 0;
    for (size_t j = 0; j < (size_t)bgen_metafile_npartitions(mf); ++j) {
        vm = bgen_read_partition(mf, j, &nvars);
        for (size_t l = 0; l < (size_t)nvars; ++l) {
            vg = bgen_open_genotype(bgen, vm[l].vaddr);
            if (bgen_phased(vg) != phased[i])
                return 1;
            bgen_close_genotype(vg);
            ++i;
        }
        bgen_free_partition(vm, nvars);
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

    int nsamples = bgen_nsamples(bgen);
    size_t jj = 0;
    for (size_t j = 0; j < (size_t)bgen_metafile_npartitions(mf); ++j) {
        vm = bgen_read_partition(mf, j, &nvars);
        for (size_t l = 0; l < (size_t)nvars; ++l) {
            vg = bgen_open_genotype(bgen, vm[l].vaddr);

            double *probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(double));
            double *p = probabilities;
            bgen_read_genotype(bgen, vg, probabilities);

            for (j = 0; j < (size_t)nsamples; ++j) {

                if (ploidys[jj] != bgen_ploidy(vg, j))
                    return 1;

                if (bgen_missing(vg, j) != 0)
                    return 1;

                for (size_t ii = 0; ii < (size_t)bgen_ncombs(vg); ++ii) {
                    if (*rp != *p && !(isnan(*rp) && isnan(*p)))
                        return 1;
                    ++rp;
                    ++p;
                }
                ++jj;
            }
            free(probabilities);
            bgen_close_genotype(vg);
        }
        bgen_free_partition(vm, nvars);
    }

    bgen_close_metafile(mf);
    bgen_close(bgen);

    return 0;
}

int main()
{
    if (test_file())
        return 1;

    if (test_geno())
        return 1;

    return 0;
}
