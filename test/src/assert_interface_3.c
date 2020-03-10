#include "bgen/bgen.h"
#include "cass.h"
#include <stdlib.h>

void test_open_wrong_bgen_filepath()
{
    struct bgen_file *bgen = bgen_file_open("wrong.metadata");
    cass_cond(bgen == NULL);
}

void test_bgen_file_haplotypes()
{
    struct bgen_file *bgen = bgen_file_open("data/haplotypes.bgen");

    cass_cond(bgen != NULL);
    cass_equal_int(bgen_file_nsamples(bgen), 4);
    cass_equal_int(bgen_file_nvariants(bgen), 4);
    cass_equal_int(bgen_file_contain_samples(bgen), 1);

    struct bgen_samples *samples = bgen_file_read_samples(bgen, 0);
    cass_cond(samples != NULL);
    cass_cond(bgen_str_equal(BGEN_STR("sample_0"), *bgen_samples_get(samples, 0)));
    bgen_samples_free(samples);

    bgen_file_close(bgen);
}

void test_create_meteadata_haplotypes()
{
    struct bgen_file *bgen = bgen_file_open("data/haplotypes.bgen");

    struct bgen_mf *mf = bgen_metafile_create(bgen, "haplotypes.bgen.metadata.1", 4, 0);
    cass_cond(mf != NULL);

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 4);

    int nvars;
    struct bgen_vm *vm = bgen_metafile_read_partition(mf, 0, &nvars);

    struct bgen_genotype *vg = bgen_file_open_genotype(bgen, vm->genotype_offset);
    cass_cond(vg != NULL);

    cass_equal_int(bgen_genotype_nalleles(vg), 2);
    for (int i = 0; i < 4; ++i) {
        cass_equal_int(bgen_genotype_missing(vg, i), 0);
        cass_equal_int(bgen_genotype_ploidy(vg, i), 2);
    }
    cass_equal_int(bgen_genotype_min_ploidy(vg), 2);
    cass_equal_int(bgen_genotype_max_ploidy(vg), 2);
    cass_equal_int(bgen_genotype_ncombs(vg), 4);
    cass_equal_int(bgen_genotype_phased(vg), 1);

    bgen_genotype_close(vg);
    bgen_free_partition(vm, nvars);

    cass_equal_int(bgen_mf_close(mf), 0);
    bgen_file_close(bgen);
}

void test_genotype_haplotypes_by_creating_metadata()
{
    struct bgen_file *bgen = bgen_file_open("data/haplotypes.bgen");

    struct bgen_mf *mf = bgen_metafile_create(bgen, "haplotypes.bgen.metadata.2", 4, 0);
    cass_cond(mf != NULL);

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 4);

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
    for (int i = 0; i < 4; ++i) {
        int nvars;
        struct bgen_vm *vm = bgen_metafile_read_partition(mf, i, &nvars);

        for (int ii = 0; ii < nvars; ++ii) {
            struct bgen_genotype *vg = bgen_file_open_genotype(bgen, vm[ii].genotype_offset);
            cass_cond(vg != NULL);

            cass_equal_int(bgen_genotype_nalleles(vg), nalleles[i]);
            for (size_t j = 0; j < (size_t)nsamples; ++j) {
                cass_equal_int(bgen_genotype_missing(vg, j), 0);
                cass_equal_int(bgen_genotype_ploidy(vg, j), 2);
            }
            cass_equal_int(bgen_genotype_min_ploidy(vg), min_ploidy[i]);
            cass_equal_int(bgen_genotype_max_ploidy(vg), max_ploidy[i]);
            cass_equal_int(bgen_genotype_ncombs(vg), ncombs[i]);
            cass_equal_int(bgen_genotype_phased(vg), phased[i]);

            double *ptr = malloc(nsamples * ncombs[i] * sizeof(double));
            cass_equal_int(bgen_genotype_read(vg, ptr), 0);
            double *p = ptr;
            for (int j = 0; j < nsamples; ++j) {
                for (int c = 0; c < ncombs[i]; ++c) {
                    cass_close(*p, *(probs_ptr++));
                    ++p;
                }
            }
            free(ptr);

            bgen_genotype_close(vg);
        }
        bgen_free_partition(vm, nvars);
    }

    cass_equal_int(bgen_mf_close(mf), 0);
    bgen_file_close(bgen);
}

void test_genotype_haplotypes_by_loading_metadata()
{
    struct bgen_file *bgen = bgen_file_open("data/haplotypes.bgen");

    struct bgen_mf *mf = bgen_open_metafile("haplotypes.bgen.metadata.2");
    cass_cond(mf != NULL);

    cass_equal_int(bgen_metafile_npartitions(mf), 4);
    cass_equal_int(bgen_metafile_nvariants(mf), 4);

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
    cass_equal_int(bgen_metafile_nvariants(mf), 4);
    double *probs_ptr = &probs[0];
    for (int i = 0; i < bgen_metafile_npartitions(mf); ++i) {
        int nvars;
        struct bgen_vm *vm = bgen_metafile_read_partition(mf, i, &nvars);

        for (int ii = 0; ii < nvars; ++ii) {
            struct bgen_genotype *vg = bgen_file_open_genotype(bgen, vm[ii].genotype_offset);
            cass_cond(vg != NULL);

            cass_equal_int(bgen_genotype_nalleles(vg), nalleles[i]);
            for (int j = 0; j < nsamples; ++j) {
                cass_equal_int(bgen_genotype_missing(vg, j), 0);
                cass_equal_int(bgen_genotype_ploidy(vg, j), 2);
            }
            cass_equal_int(bgen_genotype_min_ploidy(vg), min_ploidy[i]);
            cass_equal_int(bgen_genotype_max_ploidy(vg), max_ploidy[i]);
            cass_equal_int(bgen_genotype_ncombs(vg), ncombs[i]);
            cass_equal_int(bgen_genotype_phased(vg), phased[i]);

            double *ptr = malloc(nsamples * ncombs[i] * sizeof(double));
            cass_equal_int(bgen_genotype_read(vg, ptr), 0);
            double *p = ptr;
            for (int j = 0; j < nsamples; ++j) {
                for (int c = 0; c < ncombs[i]; ++c) {
                    cass_close(*p, *(probs_ptr++));
                    ++p;
                }
            }
            free(ptr);

            bgen_genotype_close(vg);
        }
        bgen_free_partition(vm, nvars);
    }

    cass_equal_int(bgen_mf_close(mf), 0);
    bgen_file_close(bgen);
}

int main()
{
    test_open_wrong_bgen_filepath();
    test_bgen_file_haplotypes();
    test_create_meteadata_haplotypes();
    test_genotype_haplotypes_by_creating_metadata();
    test_genotype_haplotypes_by_loading_metadata();
    return cass_status();
}
