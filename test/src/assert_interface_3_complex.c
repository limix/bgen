#include "bgen/bgen.h"
#include "cass.h"
#include <math.h>

void test_bgen_file_complex(void);
void test_create_metadata_complex(void);
void _test_genotype_complex(struct bgen_file* bgen, struct bgen_metafile* mf);
void test_genotype_complex(void);

int main(void)
{
    test_bgen_file_complex();
    test_create_metadata_complex();
    test_genotype_complex();
    return cass_status();
}

void test_bgen_file_complex(void)
{
    struct bgen_file* bgen = bgen_file_open(TEST_DATADIR "complex.23bits.bgen");

    cass_cond(bgen != NULL) cass_equal_int(bgen_file_nsamples(bgen), 4);
    cass_equal_int(bgen_file_nvariants(bgen), 10);
    cass_equal_int(bgen_file_contain_samples(bgen), 1);

    struct bgen_samples* samples = bgen_file_read_samples(bgen);
    cass_cond(samples != NULL)
        cass_cond(bgen_string_equal(BGEN_STRING("sample_0"), *bgen_samples_get(samples, 0)));
    bgen_samples_destroy(samples);

    bgen_file_close(bgen);
}

void test_create_metadata_complex(void)
{
    struct bgen_file* bgen = bgen_file_open(TEST_DATADIR "complex.23bits.bgen");

    struct bgen_metafile* mf = bgen_metafile_create(
        bgen, "assert_interface_3_complex.tmp/complex.23bits.bgen.metafile.1", 4, 0);
    cass_cond(mf != NULL);

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 10);

    struct bgen_partition const* partition = bgen_metafile_read_partition(mf, 0);

    struct bgen_variant const* vm = bgen_partition_get_variant(partition, 0);
    struct bgen_genotype*      vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
    cass_cond(vg != NULL)

        cass_equal_int(bgen_genotype_nalleles(vg), 2);
    uint8_t ploidy[] = {1, 2, 2, 2};
    bool    miss[] = {0, 0, 0, 0};
    for (uint32_t i = 0; i < bgen_file_nsamples(bgen); ++i) {
        cass_cond(bgen_genotype_missing(vg, i) == miss[i]);
        cass_cond(bgen_genotype_ploidy(vg, i) == ploidy[i]);
    }
    cass_equal_int(bgen_genotype_min_ploidy(vg), 1);
    cass_equal_int(bgen_genotype_max_ploidy(vg), 2);
    cass_equal_int(bgen_genotype_ncombs(vg), 3);
    cass_equal_int(bgen_genotype_phased(vg), 0);

    bgen_genotype_close(vg);
    bgen_partition_destroy(partition);

    cass_equal_int(bgen_metafile_close(mf), 0);
    bgen_file_close(bgen);
}

void _test_genotype_complex(struct bgen_file* bgen, struct bgen_metafile* mf)
{
    cass_cond(mf != NULL)

        cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 10);

    double probs[] = {
        1.0000, 0.0000, NAN,    1.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 0.0000, 1.0000, 1.0000, 0.0000,
        NAN,    NAN,    0.0000, 1.0000, 0.0000, 1.0000, 1.0000, 0.0000, 1.0000, 0.0000, 1.0000,
        0.0000, 0.0000, 1.0000, 1.0000, 0.0000, 0.0000, NAN,    NAN,    NAN,    1.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 1.0000, 0.0000, NAN,    NAN,    NAN,    NAN,
        1.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 0.0000,
        1.0000, 1.0000, 0.0000, 0.0000, 1.0000, NAN,    NAN,    1.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        1.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 1.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000,
        0.0000, NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000};
    double* probs_ptr = &probs[0];
    int     nalleles[] = {2, 2, 2, 3, 2, 4, 6, 7, 8, 2};
    int*    nalleles_ptr = nalleles + 0;
    int     ploidy[] = {1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 3, 3, 2,
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 4};
    int*    ploidy_ptr = ploidy + 0;
    int     min_ploidy[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 4};
    int*    min_ploidy_ptr = min_ploidy + 0;
    int     max_ploidy[] = {2, 1, 2, 2, 3, 1, 1, 1, 2, 4};
    int*    max_ploidy_ptr = max_ploidy + 0;
    int     ncombs[] = {3, 2, 4, 6, 6, 4, 6, 7, 36, 5};
    int*    ncombs_ptr = ncombs + 0;
    int     phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};
    int*    phased_ptr = phased + 0;

    for (uint32_t i = 0; i < bgen_metafile_npartitions(mf); ++i) {
        struct bgen_partition const* partition = bgen_metafile_read_partition(mf, i);

        for (uint32_t ii = 0; ii < bgen_partition_nvariants(partition); ++ii) {
            struct bgen_variant const* vm = bgen_partition_get_variant(partition, ii);
            struct bgen_genotype*      vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
            cass_cond(vg != NULL)

                cass_equal_int(bgen_genotype_nalleles(vg), *(nalleles_ptr++));
            for (uint32_t j = 0; j < bgen_file_nsamples(bgen); ++j) {
                cass_cond(!bgen_genotype_missing(vg, j));
                cass_cond(bgen_genotype_ploidy(vg, j) == *(ploidy_ptr++));
            }
            cass_equal_int(bgen_genotype_min_ploidy(vg), *(min_ploidy_ptr++));
            cass_equal_int(bgen_genotype_max_ploidy(vg), *(max_ploidy_ptr++));
            cass_equal_int(bgen_genotype_ncombs(vg), *(ncombs_ptr++));
            cass_equal_int(bgen_genotype_phased(vg), *(phased_ptr++));

            double* ptr =
                malloc(bgen_file_nsamples(bgen) * bgen_genotype_ncombs(vg) * sizeof(double));

            cass_equal_int(bgen_genotype_read(vg, ptr), 0);
            double* p = ptr;
            for (uint32_t j = 0; j < bgen_file_nsamples(bgen); ++j) {
                for (unsigned c = 0; c < bgen_genotype_ncombs(vg); ++c) {
                    cass_close(*p, *(probs_ptr++));
                    ++p;
                }
            }
            free(ptr);

            bgen_genotype_close(vg);
        }
        bgen_partition_destroy(partition);
    }
}

void test_genotype_complex(void)
{
    {
        struct bgen_file*     bgen = bgen_file_open(TEST_DATADIR "complex.23bits.bgen");
        struct bgen_metafile* mf = bgen_metafile_create(
            bgen, "assert_interface_3_complex.tmp/complex.23bits.bgen.metafile.2", 4, 0);

        _test_genotype_complex(bgen, mf);
        cass_equal_int(bgen_metafile_close(mf), 0);
        bgen_file_close(bgen);
    }

    {
        struct bgen_file*     bgen = bgen_file_open(TEST_DATADIR "complex.23bits.bgen");
        struct bgen_metafile* mf = bgen_metafile_open(
            "assert_interface_3_complex.tmp/complex.23bits.bgen.metafile.2");

        _test_genotype_complex(bgen, mf);
        cass_equal_int(bgen_metafile_close(mf), 0);
        bgen_file_close(bgen);
    }
}
