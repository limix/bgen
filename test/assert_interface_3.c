#include "bgen.h"
#include "cass.h"
#include <stdlib.h>

#define TEST_CASE(x)                                                                   \
    if (x)                                                                             \
        return 1;

int test_open_wrong_bgen_filepath()
{
    struct bgen_file *bgen = bgen_open("wrong.metadata");
    assert_null(bgen);
    bgen_close(bgen);
    return 0;
}

int test_bgen_file_haplotypes()
{
    struct bgen_file *bgen = bgen_open("data/haplotypes.bgen");

    assert_not_null(bgen);
    assert_equal_int(bgen_nsamples(bgen), 4);
    assert_equal_int(bgen_nvariants(bgen), 4);
    assert_equal_int(bgen_contain_samples(bgen), 1);

    struct bgen_str *samples = bgen_read_samples(bgen, 0);
    assert_not_null(samples);
    assert_strncmp(samples[0].str, "sample_0", samples[0].len);
    bgen_free_samples(bgen, samples);

    bgen_close(bgen);

    return 0;
}

int test_create_meteadata_haplotypes()
{
    struct bgen_file *bgen = bgen_open("data/haplotypes.bgen");

    struct bgen_mf *mf = bgen_create_metafile(bgen, "haplotypes.bgen.metadata.1", 4, 0);
    assert_not_null(mf);

    assert_equal_int(bgen_metafile_nparts(mf), 4);
    assert_equal_int(bgen_metafile_nvars(mf), 4);

    int nvars;
    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvars);

    struct bgen_vg *vg = bgen_open_genotype(bgen, vm);
    assert_not_null(vg);

    assert_equal_int(bgen_nalleles(vg), 2);
    for (size_t i = 0; i < 4; ++i) {
        assert_equal_int(bgen_missing(vg, i), 0);
        assert_equal_int(bgen_ploidy(vg, i), 2);
    }
    assert_equal_int(bgen_min_ploidy(vg), 2);
    assert_equal_int(bgen_max_ploidy(vg), 2);
    assert_equal_int(bgen_ncombs(vg), 4);
    assert_equal_int(bgen_phased(vg), 1);

    bgen_close_genotype(vg);
    bgen_free_partition(vm, nvars);

    assert_equal_int(bgen_close_metafile(mf), 0);
    bgen_close(bgen);
    return 0;
}

int test_genotype_haplotypes_by_creating_metadata()
{
    struct bgen_file *bgen = bgen_open("data/haplotypes.bgen");

    struct bgen_mf *mf = bgen_create_metafile(bgen, "haplotypes.bgen.metadata.2", 4, 0);
    assert_not_null(mf);

    assert_equal_int(bgen_metafile_nparts(mf), 4);
    assert_equal_int(bgen_metafile_nvars(mf), 4);

    int nalleles[] = {2, 2, 2, 2};
    int min_ploidy[] = {2, 2, 2, 2};
    int max_ploidy[] = {2, 2, 2, 2};
    int ncombs[] = {4, 4, 4, 4};
    int phased[] = {1, 1, 1, 1};
    int nsamples = 4;
    double probs[] = {
        1.00000000000000000000, 0.00000000000000000000, 1.00000000000000000000,
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
    double *probs_ptr = &probs[0];
    for (size_t i = 0; i < 4; ++i) {
        int nvars;
        struct bgen_vm *vm = bgen_read_partition(mf, i, &nvars);

        for (int ii = 0; ii < nvars; ++ii) {
            struct bgen_vg *vg = bgen_open_genotype(bgen, vm + ii);
            assert_not_null(vg);

            assert_equal_int(bgen_nalleles(vg), nalleles[i]);
            for (size_t j = 0; j < (size_t)nsamples; ++j) {
                assert_equal_int(bgen_missing(vg, j), 0);
                assert_equal_int(bgen_ploidy(vg, j), 2);
            }
            assert_equal_int(bgen_min_ploidy(vg), min_ploidy[i]);
            assert_equal_int(bgen_max_ploidy(vg), max_ploidy[i]);
            assert_equal_int(bgen_ncombs(vg), ncombs[i]);
            assert_equal_int(bgen_phased(vg), phased[i]);

            double *ptr = malloc(nsamples * ncombs[i] * sizeof(double));
            assert_equal_int(bgen_read_genotype(bgen, vg, ptr), 0);
            double *p = ptr;
            for (int j = 0; j < nsamples; ++j) {
                for (int c = 0; c < ncombs[i]; ++c) {
                    assert_almost_equal(*p, *(probs_ptr++));
                    ++p;
                }
                printf("\n");
            }
            free(ptr);

            bgen_close_genotype(vg);
        }
        bgen_free_partition(vm, nvars);
    }

    assert_equal_int(bgen_close_metafile(mf), 0);
    bgen_close(bgen);
    return 0;
}

int main()
{

    TEST_CASE(test_open_wrong_bgen_filepath());
    TEST_CASE(test_bgen_file_haplotypes());
    TEST_CASE(test_create_meteadata_haplotypes());
    TEST_CASE(test_genotype_haplotypes_by_creating_metadata());

    return 0;
}
