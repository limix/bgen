#include "bgen/bgen.h"
#include "cass.h"
#include <math.h>

void test_complex64(void);
void test_complex32(void);

int main(void)
{
    test_complex64();
    test_complex32();
    return cass_status();
}

void test_complex64(void)
{
    const char        filename[] = TEST_DATADIR "complex.23bits.bgen";
    struct bgen_file* bgen;
    uint32_t          nsamples, nvariants;
    double*           probabilities;

    cass_cond((bgen = bgen_file_open(filename)) != NULL);
    cass_cond((nsamples = bgen_file_nsamples(bgen)) == 4);
    cass_cond((nvariants = bgen_file_nvariants(bgen)) == 10);

    struct bgen_samples* samples = bgen_file_read_samples(bgen);

    cass_cond(bgen_string_equal(BGEN_STRING("sample_0"), *bgen_samples_get(samples, 0)));
    cass_cond(bgen_string_equal(BGEN_STRING("sample_3"), *bgen_samples_get(samples, 3)));

    bgen_samples_destroy(samples);

    struct bgen_metafile* mf =
        bgen_metafile_create(bgen, "complex.tmp/complex.23bits.bgen.metafile", 1, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition(mf, 0);
    cass_cond(nvariants == 10);

    struct bgen_variant const* vm = bgen_partition_get_variant(partition, 0);
    cass_cond(bgen_string_equal(BGEN_STRING("V1"), *vm->rsid));
    vm = bgen_partition_get_variant(partition, 9);
    cass_cond(bgen_string_equal(BGEN_STRING("M10"), *vm->rsid));

    uint32_t position[] = {1, 2, 3, 4, 5, 7, 7, 8, 9, 10};
    uint16_t correct_nalleles[] = {2, 2, 2, 3, 2, 4, 6, 7, 8, 2};
    char*    allele_ids[] = {"A",    "G",     "A",      "G",       "A",     "G",  "A",   "G",
                             "T",    "A",     "G",      "A",       "G",     "GT", "GTT", "A",
                             "G",    "GT",    "GTT",    "GTTT",    "GTTTT", "A",  "G",   "GT",
                             "GTT",  "GTTT",  "GTTTT",  "GTTTTT",  "A",     "G",  "GT",  "GTT",
                             "GTTT", "GTTTT", "GTTTTT", "GTTTTTT", "A",     "G"};

    uint32_t jj = 0;
    for (uint32_t i = 0; i < nvariants; ++i) {
        vm = bgen_partition_get_variant(partition, i);
        cass_cond(vm->nalleles == correct_nalleles[i]);
        cass_cond(vm->position == position[i]);

        for (uint16_t j = 0; j < vm->nalleles; ++j) {

            cass_cond(bgen_string_equal(BGEN_STRING(allele_ids[jj]), *vm->allele_ids[j]));
            ++jj;
        }
    }

    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};

    int    ploidys[] = {1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 3, 3, 2,
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

    jj = 0;
    for (uint32_t i = 0; i < nvariants; ++i) {
        vm = bgen_partition_get_variant(partition, i);
        struct bgen_genotype* vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
        cass_cond(bgen_genotype_phased(vg) == phased[i]);

        probabilities = malloc(nsamples * bgen_genotype_ncombs(vg) * sizeof(*probabilities));
        bgen_genotype_read(vg, probabilities);

        double* p = probabilities;
        for (uint16_t j = 0; j < nsamples; ++j) {

            cass_cond(ploidys[jj] == bgen_genotype_ploidy(vg, j));
            cass_cond(bgen_genotype_missing(vg, j) == 0);

            for (unsigned ii = 0; ii < bgen_genotype_ncombs(vg); ++ii) {
                cass_cond(!(*rp != *p && !(isnan(*rp) && isnan(*p))));
                ++rp;
                ++p;
            }
            ++jj;
        }
        bgen_genotype_close(vg);
        free(probabilities);
    }

    bgen_partition_destroy(partition);
    cass_cond(bgen_metafile_close(mf) == 0);
    bgen_file_close(bgen);
}

void test_complex32(void)
{
    const char        filename[] = TEST_DATADIR "complex.23bits.bgen";
    struct bgen_file* bgen;
    uint32_t          nsamples, nvariants;
    float*            probabilities;

    cass_cond((bgen = bgen_file_open(filename)) != NULL);
    cass_cond((nsamples = bgen_file_nsamples(bgen)) == 4);
    cass_cond((nvariants = bgen_file_nvariants(bgen)) == 10);

    struct bgen_samples* samples = bgen_file_read_samples(bgen);

    cass_cond(bgen_string_equal(BGEN_STRING("sample_0"), *bgen_samples_get(samples, 0)));
    cass_cond(bgen_string_equal(BGEN_STRING("sample_3"), *bgen_samples_get(samples, 3)));

    bgen_samples_destroy(samples);

    struct bgen_metafile* mf =
        bgen_metafile_create(bgen, "complex.tmp/complex.23bits.bgen.metafile", 1, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition(mf, 0);
    cass_cond(nvariants == 10);

    struct bgen_variant const* vm = bgen_partition_get_variant(partition, 0);
    cass_cond(bgen_string_equal(BGEN_STRING("V1"), *vm->rsid));
    vm = bgen_partition_get_variant(partition, 9);
    cass_cond(bgen_string_equal(BGEN_STRING("M10"), *vm->rsid));

    uint32_t position[] = {1, 2, 3, 4, 5, 7, 7, 8, 9, 10};
    uint16_t correct_nalleles[] = {2, 2, 2, 3, 2, 4, 6, 7, 8, 2};
    char*    allele_ids[] = {"A",    "G",     "A",      "G",       "A",     "G",  "A",   "G",
                             "T",    "A",     "G",      "A",       "G",     "GT", "GTT", "A",
                             "G",    "GT",    "GTT",    "GTTT",    "GTTTT", "A",  "G",   "GT",
                             "GTT",  "GTTT",  "GTTTT",  "GTTTTT",  "A",     "G",  "GT",  "GTT",
                             "GTTT", "GTTTT", "GTTTTT", "GTTTTTT", "A",     "G"};

    uint32_t jj = 0;
    for (uint32_t i = 0; i < nvariants; ++i) {
        vm = bgen_partition_get_variant(partition, i);
        cass_cond(vm->nalleles == correct_nalleles[i]);
        cass_cond(vm->position == position[i]);

        for (uint16_t j = 0; j < vm->nalleles; ++j) {

            cass_cond(bgen_string_equal(BGEN_STRING(allele_ids[jj]), *vm->allele_ids[j]));
            ++jj;
        }
    }

    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};

    int   ploidys[] = {1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 3, 3, 2,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 4};
    float real_probs[] = {
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

    float* rp = real_probs;

    jj = 0;
    for (uint32_t i = 0; i < nvariants; ++i) {
        vm = bgen_partition_get_variant(partition, i);
        struct bgen_genotype* vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
        cass_cond(bgen_genotype_phased(vg) == phased[i]);

        probabilities = malloc(nsamples * bgen_genotype_ncombs(vg) * sizeof(*probabilities));
        bgen_genotype_read32(vg, probabilities);

        float* p = probabilities;
        for (uint16_t j = 0; j < nsamples; ++j) {

            cass_cond(ploidys[jj] == bgen_genotype_ploidy(vg, j));
            cass_cond(bgen_genotype_missing(vg, j) == 0);

            for (unsigned ii = 0; ii < bgen_genotype_ncombs(vg); ++ii) {
                cass_cond(!(*rp != *p && !(isnan(*rp) && isnan(*p))));
                ++rp;
                ++p;
            }
            ++jj;
        }
        bgen_genotype_close(vg);
        free(probabilities);
    }

    bgen_partition_destroy(partition);
    cass_cond(bgen_metafile_close(mf) == 0);
    bgen_file_close(bgen);
}
