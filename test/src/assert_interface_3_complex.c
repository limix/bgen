#include "bgen/bgen.h"
#include "cass.h"
#include <math.h>
#include <stdlib.h>

void test_bgen_file_complex()
{
    struct bgen_file *bgen = bgen_open("data/complex.23bits.bgen");

    cass_cond(bgen != NULL)
    cass_equal_int(bgen_nsamples(bgen), 4);
    cass_equal_int(bgen_nvariants(bgen), 10);
    cass_equal_int(bgen_contain_samples(bgen), 1);

    struct bgen_str *samples = bgen_read_samples(bgen, 0);
    cass_cond(samples != NULL)
    cass_cond(bgen_str_equal(BGEN_STR("sample_0"), samples[0]));
    bgen_free_samples(bgen, samples);

    bgen_close(bgen);
}

void test_create_metadata_complex()
{
    struct bgen_file *bgen = bgen_open("data/complex.23bits.bgen");

    struct bgen_mf *mf =
        bgen_create_metafile(bgen, "complex.23bits.bgen.metadata.1", 4, 0);
    cass_cond(mf != NULL)

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 10);

    int nvars;
    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvars);

    struct bgen_vg *vg = bgen_open_genotype(bgen, vm->vaddr);
    cass_cond(vg != NULL)

    cass_equal_int(bgen_nalleles(vg), 2);
    int ploidy[] = {1, 2, 2, 2};
    int miss[] = {0, 0, 0, 0};
    for (size_t i = 0; i < (size_t)bgen_nsamples(bgen); ++i) {
        cass_equal_int(bgen_missing(vg, i), miss[i]);
        cass_equal_int(bgen_ploidy(vg, i), ploidy[i]);
    }
    cass_equal_int(bgen_min_ploidy(vg), 1);
    cass_equal_int(bgen_max_ploidy(vg), 2);
    cass_equal_int(bgen_ncombs(vg), 3);
    cass_equal_int(bgen_phased(vg), 0);

    bgen_close_genotype(vg);
    bgen_free_partition(vm, nvars);

    cass_equal_int(bgen_close_metafile(mf), 0);
    bgen_close(bgen);
}

void _test_genotype_complex(struct bgen_file *bgen, struct bgen_mf *mf)
{
    cass_cond(mf != NULL)

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 10);

    double probs[] = {
        1.0000, 0.0000, NAN,    1.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000,
        1.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 0.0000, 1.0000,
        1.0000, 0.0000, NAN,    NAN,    0.0000, 1.0000, 0.0000, 1.0000, 1.0000, 0.0000,
        1.0000, 0.0000, 1.0000, 0.0000, 0.0000, 1.0000, 1.0000, 0.0000, 0.0000, NAN,
        NAN,    NAN,    1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000,
        1.0000, 0.0000, NAN,    NAN,    NAN,    NAN,    1.0000, 0.0000, 1.0000, 0.0000,
        1.0000, 0.0000, 1.0000, 0.0000, 1.0000, 0.0000, 0.0000, 1.0000, 1.0000, 0.0000,
        0.0000, 1.0000, NAN,    NAN,    1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 1.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        1.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000,
        0.0000, 0.0000, NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,    NAN,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000};
    double *probs_ptr = &probs[0];
    int nalleles[] = {2, 2, 2, 3, 2, 4, 6, 7, 8, 2};
    int *nalleles_ptr = nalleles + 0;
    int ploidy[] = {1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 3, 3, 2,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 4};
    int *ploidy_ptr = ploidy + 0;
    int min_ploidy[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 4};
    int *min_ploidy_ptr = min_ploidy + 0;
    int max_ploidy[] = {2, 1, 2, 2, 3, 1, 1, 1, 2, 4};
    int *max_ploidy_ptr = max_ploidy + 0;
    int ncombs[] = {3, 2, 4, 6, 6, 4, 6, 7, 36, 5};
    int *ncombs_ptr = ncombs + 0;
    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};
    int *phased_ptr = phased + 0;

    for (size_t i = 0; i < (size_t)bgen_metafile_npartitions(mf); ++i) {
        int nvars;
        struct bgen_vm *vm = bgen_read_partition(mf, i, &nvars);

        for (int ii = 0; ii < nvars; ++ii) {
            struct bgen_vg *vg = bgen_open_genotype(bgen, vm[ii].vaddr);
            cass_cond(vg != NULL)

            cass_equal_int(bgen_nalleles(vg), *(nalleles_ptr++));
            for (size_t j = 0; j < (size_t)bgen_nsamples(bgen); ++j) {
                cass_equal_int(bgen_missing(vg, j), 0);
                cass_equal_int(bgen_ploidy(vg, j), *(ploidy_ptr++));
            }
            cass_equal_int(bgen_min_ploidy(vg), *(min_ploidy_ptr++));
            cass_equal_int(bgen_max_ploidy(vg), *(max_ploidy_ptr++));
            cass_equal_int(bgen_ncombs(vg), *(ncombs_ptr++));
            cass_equal_int(bgen_phased(vg), *(phased_ptr++));

            double *ptr =
                malloc(bgen_nsamples(bgen) * bgen_ncombs(vg) * sizeof(double));

            cass_equal_int(bgen_read_genotype(bgen, vg, ptr), 0);
            double *p = ptr;
            for (int j = 0; j < bgen_nsamples(bgen); ++j) {
                for (int c = 0; c < bgen_ncombs(vg); ++c) {
                    cass_close(*p, *(probs_ptr++));
                    ++p;
                }
            }
            free(ptr);

            bgen_close_genotype(vg);
        }
        bgen_free_partition(vm, nvars);
    }
}

void test_genotype_complex()
{
    {
        struct bgen_file *bgen = bgen_open("data/complex.23bits.bgen");
        struct bgen_mf *mf =
            bgen_create_metafile(bgen, "complex.23bits.bgen.metadata.2", 4, 0);

        _test_genotype_complex(bgen, mf);
        cass_equal_int(bgen_close_metafile(mf), 0);
        bgen_close(bgen);
    }

    {
        struct bgen_file *bgen = bgen_open("data/complex.23bits.bgen");
        struct bgen_mf *mf = bgen_open_metafile("complex.23bits.bgen.metadata.2");

        _test_genotype_complex(bgen, mf);
        cass_equal_int(bgen_close_metafile(mf), 0);
        bgen_close(bgen);
    }
}

int main()
{
    test_bgen_file_complex();
    test_create_metadata_complex();
    test_genotype_complex();
    return cass_status();
}