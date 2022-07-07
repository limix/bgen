#include "bgen/bgen.h"
#include "cass.h"

void test_open_wrong_bgen_filepath(void);
void test_bgen_file_haplotypes(void);
void test_create_meteadata_haplotypes(void);
void test_genotype_haplotypes_by_creating_metadata(void);
void test_genotype_haplotypes_by_loading_metadata(void);

int main(void)
{
    test_open_wrong_bgen_filepath();
    test_bgen_file_haplotypes();
    test_create_meteadata_haplotypes();
    test_genotype_haplotypes_by_creating_metadata();
    test_genotype_haplotypes_by_loading_metadata();
    return cass_status();
}

void test_open_wrong_bgen_filepath(void)
{
    struct bgen_file* bgen = bgen_file_open(TEST_DATADIR "wrong.metafile");
    cass_cond(bgen == NULL);
}

void test_bgen_file_haplotypes(void)
{
    struct bgen_file* bgen = bgen_file_open(TEST_DATADIR "haplotypes.bgen");

    cass_cond(bgen != NULL);
    cass_equal_int(bgen_file_nsamples(bgen), 4);
    cass_equal_int(bgen_file_nvariants(bgen), 4);
    cass_equal_int(bgen_file_contain_samples(bgen), 1);

    struct bgen_samples* samples = bgen_file_read_samples(bgen);
    cass_cond(samples != NULL);
    cass_cond(bgen_string_equal(BGEN_STRING("sample_0"), *bgen_samples_get(samples, 0)));
    bgen_samples_destroy(samples);

    bgen_file_close(bgen);
}

void test_create_meteadata_haplotypes(void)
{
    struct bgen_file* bgen = bgen_file_open(TEST_DATADIR "haplotypes.bgen");

    struct bgen_metafile* mf =
        bgen_metafile_create(bgen, "assert_interface_3.tmp/haplotypes.bgen.metafile.1", 4, 0);
    cass_cond(mf != NULL);

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 4);

    struct bgen_partition const* partition = bgen_metafile_read_partition(mf, 0);

    struct bgen_variant const* vm = bgen_partition_get_variant(partition, 0);
    struct bgen_genotype*      vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
    cass_cond(vg != NULL);

    cass_equal_int(bgen_genotype_nalleles(vg), 2);
    for (uint32_t i = 0; i < 4; ++i) {
        cass_equal_int(bgen_genotype_missing(vg, i), 0);
        cass_equal_int(bgen_genotype_ploidy(vg, i), 2);
    }
    cass_equal_int(bgen_genotype_min_ploidy(vg), 2);
    cass_equal_int(bgen_genotype_max_ploidy(vg), 2);
    cass_equal_int(bgen_genotype_ncombs(vg), 4);
    cass_equal_int(bgen_genotype_phased(vg), 1);

    bgen_genotype_close(vg);
    bgen_partition_destroy(partition);

    cass_equal_int(bgen_metafile_close(mf), 0);
    bgen_file_close(bgen);
}

void test_genotype_haplotypes_by_creating_metadata(void)
{
    struct bgen_file* bgen = bgen_file_open(TEST_DATADIR "haplotypes.bgen");

    struct bgen_metafile* mf =
        bgen_metafile_create(bgen, "assert_interface_3.tmp/haplotypes.bgen.metafile.2", 4, 0);
    cass_cond(mf != NULL);

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 4);

    int      nalleles[] = {2, 2, 2, 2};
    int      min_ploidy[] = {2, 2, 2, 2};
    int      max_ploidy[] = {2, 2, 2, 2};
    unsigned ncombs[] = {4, 4, 4, 4};
    int      phased[] = {1, 1, 1, 1};
    uint32_t nsamples = 4;
    double   probs[] = {1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000};
    double*  probs_ptr = &probs[0];
    for (uint32_t i = 0; i < 4; ++i) {
        struct bgen_partition const* partition = bgen_metafile_read_partition(mf, i);

        for (uint32_t ii = 0; ii < bgen_partition_nvariants(partition); ++ii) {
            struct bgen_variant const* vm = bgen_partition_get_variant(partition, ii);
            struct bgen_genotype*      vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
            cass_cond(vg != NULL);

            cass_equal_int(bgen_genotype_nalleles(vg), nalleles[i]);
            for (uint32_t j = 0; j < nsamples; ++j) {
                cass_equal_int(bgen_genotype_missing(vg, j), 0);
                cass_equal_int(bgen_genotype_ploidy(vg, j), 2);
            }
            cass_equal_int(bgen_genotype_min_ploidy(vg), min_ploidy[i]);
            cass_equal_int(bgen_genotype_max_ploidy(vg), max_ploidy[i]);
            cass_equal_int(bgen_genotype_ncombs(vg), ncombs[i]);
            cass_equal_int(bgen_genotype_phased(vg), phased[i]);

            double* ptr = malloc(nsamples * ncombs[i] * sizeof(double));
            cass_equal_int(bgen_genotype_read(vg, ptr), 0);
            double* p = ptr;
            for (uint32_t j = 0; j < nsamples; ++j) {
                for (unsigned c = 0; c < ncombs[i]; ++c) {
                    cass_close(*p, *(probs_ptr++));
                    ++p;
                }
            }
            free(ptr);

            bgen_genotype_close(vg);
        }
        bgen_partition_destroy(partition);
    }

    cass_equal_int(bgen_metafile_close(mf), 0);
    bgen_file_close(bgen);
}

void test_genotype_haplotypes_by_loading_metadata(void)
{
    struct bgen_file* bgen = bgen_file_open(TEST_DATADIR "haplotypes.bgen");

    struct bgen_metafile* mf =
        bgen_metafile_open("assert_interface_3.tmp/haplotypes.bgen.metafile.2");
    cass_cond(mf != NULL);

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 4);

    int      nalleles[] = {2, 2, 2, 2};
    int      min_ploidy[] = {2, 2, 2, 2};
    int      max_ploidy[] = {2, 2, 2, 2};
    unsigned ncombs[] = {4, 4, 4, 4};
    int      phased[] = {1, 1, 1, 1};
    uint32_t nsamples = 4;
    double   probs[] = {1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        0.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 1.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000,
                        1.00000000000000000000};
    cass_equal_int(bgen_metafile_nvariants(mf), 4);
    double* probs_ptr = &probs[0];
    for (uint32_t i = 0; i < bgen_metafile_npartitions(mf); ++i) {
        struct bgen_partition const* partition = bgen_metafile_read_partition(mf, i);

        for (uint32_t ii = 0; ii < bgen_partition_nvariants(partition); ++ii) {
            struct bgen_variant const* vm = bgen_partition_get_variant(partition, ii);
            struct bgen_genotype*      vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
            cass_cond(vg != NULL);

            cass_equal_int(bgen_genotype_nalleles(vg), nalleles[i]);
            for (uint32_t j = 0; j < nsamples; ++j) {
                cass_equal_int(bgen_genotype_missing(vg, j), 0);
                cass_equal_int(bgen_genotype_ploidy(vg, j), 2);
            }
            cass_equal_int(bgen_genotype_min_ploidy(vg), min_ploidy[i]);
            cass_equal_int(bgen_genotype_max_ploidy(vg), max_ploidy[i]);
            cass_equal_int(bgen_genotype_ncombs(vg), ncombs[i]);
            cass_equal_int(bgen_genotype_phased(vg), phased[i]);

            double* ptr = malloc(nsamples * ncombs[i] * sizeof(double));
            cass_equal_int(bgen_genotype_read(vg, ptr), 0);
            double* p = ptr;
            for (uint32_t j = 0; j < nsamples; ++j) {
                for (unsigned c = 0; c < ncombs[i]; ++c) {
                    cass_close(*p, *(probs_ptr++));
                    ++p;
                }
            }
            free(ptr);

            bgen_genotype_close(vg);
        }
        bgen_partition_destroy(partition);
    }

    cass_equal_int(bgen_metafile_close(mf), 0);
    bgen_file_close(bgen);
}
