#include "bgen/bgen.h"
#include "cass.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_file(void);
void test_geno(void);

int main(void)
{
    test_file();
    test_geno();
    return cass_status();
}

void test_file(void)
{
    const char        filename[] = TEST_DATADIR "complex.23bits.bgen";
    struct bgen_file* bgen;
    uint32_t          nsamples, nvariants;

    cass_cond((bgen = bgen_file_open(filename)) != NULL);
    cass_cond((nsamples = bgen_file_nsamples(bgen)) == 4);
    cass_cond((nvariants = bgen_file_nvariants(bgen)) == 10);

    struct bgen_samples* samples = bgen_file_read_samples(bgen);

    cass_cond(bgen_string_equal(BGEN_STRING("sample_0"), *bgen_samples_get(samples, 0)));
    cass_cond(bgen_string_equal(BGEN_STRING("sample_3"), *bgen_samples_get(samples, 3)));

    bgen_samples_destroy(samples);

    struct bgen_metafile* mf =
        bgen_metafile_create(bgen, "open_genotype.tmp/complex.23bits.bgen.metadata.2", 1, 0);
    struct bgen_partition const* partition = bgen_metafile_read_partition(mf, 0);

    struct bgen_variant const* vm = bgen_partition_get_variant(partition, 0);
    cass_cond(bgen_string_equal(BGEN_STRING("V1"), *vm->rsid));

    vm = bgen_partition_get_variant(partition, 9);
    cass_cond(bgen_string_equal(BGEN_STRING("M10"), *vm->rsid));

    vm = bgen_partition_get_variant(partition, 0);
    struct bgen_genotype* vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
    cass_cond(bgen_genotype_nalleles(vg) == 2);
    bgen_genotype_close(vg);

    uint32_t position[] = {1, 2, 3, 4, 5, 7, 7, 8, 9, 10};
    int      correct_nalleles[] = {2, 2, 2, 3, 2, 4, 6, 7, 8, 2};
    char*    allele_ids[] = {"A",    "G",     "A",      "G",       "A",     "G",  "A",   "G",
                             "T",    "A",     "G",      "A",       "G",     "GT", "GTT", "A",
                             "G",    "GT",    "GTT",    "GTTT",    "GTTTT", "A",  "G",   "GT",
                             "GTT",  "GTTT",  "GTTTT",  "GTTTTT",  "A",     "G",  "GT",  "GTT",
                             "GTTT", "GTTTT", "GTTTTT", "GTTTTTT", "A",     "G"};

    size_t jj = 0;
    for (uint32_t i = 0; i < nvariants; ++i) {
        vm = bgen_partition_get_variant(partition, i);
        cass_cond(vm->nalleles == correct_nalleles[i]);
        cass_cond(vm->position == position[i]);
        for (int j = 0; j < vm->nalleles; ++j) {

            cass_cond(bgen_string_equal(BGEN_STRING(allele_ids[jj]), *vm->allele_ids[j]));
            ++jj;
        }
    }
    bgen_partition_destroy(partition);
    cass_cond(bgen_metafile_close(mf) == 0);
    bgen_file_close(bgen);
}

void test_geno(void)
{
    const char        filename[] = TEST_DATADIR "complex.23bits.bgen";
    struct bgen_file* bgen;

    cass_cond((bgen = bgen_file_open(filename)) != NULL);

    struct bgen_metafile* mf =
        bgen_metafile_create(bgen, "open_genotype.tmp/complex.23bits.bgen.og.metafile", 3, 0);

    cass_cond(bgen_metafile_npartitions(mf) == 3);
    cass_cond(bgen_metafile_nvariants(mf) == 10);

    struct bgen_partition const* partition = bgen_metafile_read_partition(mf, 0);
    struct bgen_variant const*   vm = bgen_partition_get_variant(partition, 0);

    struct bgen_genotype* vg = bgen_file_open_genotype(bgen, vm->genotype_offset);

    cass_cond(bgen_genotype_nalleles(vg) == 2);
    cass_cond(bgen_genotype_missing(vg, 0) == 0);
    cass_cond(bgen_genotype_missing(vg, 1) == 0);
    cass_cond(bgen_genotype_missing(vg, 2) == 0);
    cass_cond(bgen_genotype_ploidy(vg, 0) == 1);
    cass_cond(bgen_genotype_ploidy(vg, 1) == 2);
    cass_cond(bgen_genotype_ploidy(vg, 2) == 2);
    cass_cond(bgen_genotype_min_ploidy(vg) == 1);
    cass_cond(bgen_genotype_max_ploidy(vg) == 2);
    cass_cond(bgen_genotype_ncombs(vg) == 3);
    cass_cond(bgen_genotype_phased(vg) == 0);

    bgen_genotype_close(vg);

    vm = bgen_partition_get_variant(partition, 1);
    vg = bgen_file_open_genotype(bgen, vm->genotype_offset);

    cass_cond(bgen_genotype_nalleles(vg) == 2);
    cass_cond(bgen_genotype_missing(vg, 0) == 0);
    cass_cond(bgen_genotype_missing(vg, 1) == 0);
    cass_cond(bgen_genotype_missing(vg, 2) == 0);
    cass_cond(bgen_genotype_ploidy(vg, 0) == 1);
    cass_cond(bgen_genotype_ploidy(vg, 1) == 1);
    cass_cond(bgen_genotype_ploidy(vg, 2) == 1);
    cass_cond(bgen_genotype_min_ploidy(vg) == 1);
    cass_cond(bgen_genotype_max_ploidy(vg) == 1);
    cass_cond(bgen_genotype_ncombs(vg) == 2);
    cass_cond(bgen_genotype_phased(vg) == 1);

    bgen_genotype_close(vg);

    bgen_partition_destroy(partition);

    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};

    size_t i = 0;
    for (uint32_t j = 0; j < bgen_metafile_npartitions(mf); ++j) {
        partition = bgen_metafile_read_partition(mf, j);
        for (uint32_t l = 0; l < bgen_partition_nvariants(partition); ++l) {
            vm = bgen_partition_get_variant(partition, l);
            vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
            cass_cond(bgen_genotype_phased(vg) == phased[i]);
            bgen_genotype_close(vg);
            ++i;
        }
        bgen_partition_destroy(partition);
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

    double* rp = real_probs;

    uint32_t nsamples = bgen_file_nsamples(bgen);
    size_t   jj = 0;
    for (uint32_t j = 0; j < bgen_metafile_npartitions(mf); ++j) {
        partition = bgen_metafile_read_partition(mf, j);
        for (uint32_t l = 0; l < bgen_partition_nvariants(partition); ++l) {
            vm = bgen_partition_get_variant(partition, l);
            vg = bgen_file_open_genotype(bgen, vm->genotype_offset);

            double* probabilities =
                malloc(nsamples * bgen_genotype_ncombs(vg) * sizeof(*probabilities));
            double* p = probabilities;
            bgen_genotype_read(vg, probabilities);

            for (j = 0; j < nsamples; ++j) {

                cass_cond(ploidys[jj] == bgen_genotype_ploidy(vg, j));
                cass_cond(bgen_genotype_missing(vg, j) == 0);

                for (size_t ii = 0; ii < bgen_genotype_ncombs(vg); ++ii) {
                    cass_cond(!(*rp != *p && !(isnan(*rp) && isnan(*p))));
                    ++rp;
                    ++p;
                }
                ++jj;
            }
            free(probabilities);
            bgen_genotype_close(vg);
        }
        bgen_partition_destroy(partition);
    }

    cass_cond(bgen_metafile_close(mf) == 0);
    bgen_file_close(bgen);
}
