#include "bgen/bgen.h"
#include "cass.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_file(void);
void test_geno(void);

int main()
{
    test_file();
    test_geno();
    return cass_status();
}

void test_file(void)
{
    size_t i, jj;
    const char filename[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;
    int nsamples, nvariants;

    cass_cond((bgen = bgen_open(filename)) != NULL);
    cass_cond((nsamples = bgen_nsamples(bgen)) == 4);
    cass_cond((nvariants = bgen_nvariants(bgen)) == 10);

    struct bgen_str *sample_ids = bgen_read_samples(bgen, 0);

    cass_cond(strncmp("sample_0", sample_ids[0].str, sample_ids[0].len) == 0);
    cass_cond(strncmp("sample_3", sample_ids[3].str, sample_ids[3].len) == 0);

    free(sample_ids);

    /* struct bgen_vi *index; */
    /* struct bgen_var *variants = bgen_read_variants_metadata(bgen, &index, 0); */
    struct bgen_mf* mf = bgen_create_metafile(bgen, "complex.23bits.bgen.metadata.2", 1, 0);
    struct bgen_vm* vm = bgen_read_partition(mf, 0, &nvariants);

    cass_cond(strncmp("V1", vm[0].rsid.str, vm[0].rsid.len) == 0);
    cass_cond(strncmp("M10", vm[9].rsid.str, vm[9].rsid.len) == 0);

    struct bgen_vg* vg = bgen_open_genotype(bgen, vm[0].vaddr);
    /* printf("\nnalleles: %d\n", bgen_nalleles(vg)); */
    cass_cond(bgen_nalleles(vg) == 2);
    bgen_close_genotype(vg);


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

            cass_cond(strncmp(allele_ids[jj], vm[i].allele_ids[j].str,
                        vm[i].allele_ids[j].len) == 0);
            ++jj;
        }
    }
    bgen_free_partition(vm, nvariants);
    cass_cond(bgen_close_metafile(mf) == 0);
    bgen_close(bgen);
}

void test_geno(void)
{
    const char filename[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;

    cass_cond((bgen = bgen_open(filename)) != NULL);

    struct bgen_mf *mf =
        bgen_create_metafile(bgen, "complex.23bits.bgen.og.metafile", 3, 0);

    cass_cond(bgen_metafile_npartitions(mf) == 3);
    cass_cond(bgen_metafile_nvariants(mf) == 10);

    int nvars;
    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvars);

    struct bgen_vg *vg = bgen_open_genotype(bgen, vm[0].vaddr);

    cass_cond(bgen_nalleles(vg) == 2);
    cass_cond(bgen_missing(vg, 0) == 0);
    cass_cond(bgen_missing(vg, 1) == 0);
    cass_cond(bgen_missing(vg, 2) == 0);
    cass_cond(bgen_ploidy(vg, 0) == 1);
    cass_cond(bgen_ploidy(vg, 1) == 2);
    cass_cond(bgen_ploidy(vg, 2) == 2);
    cass_cond(bgen_min_ploidy(vg) == 1);
    cass_cond(bgen_max_ploidy(vg) == 2);
    cass_cond(bgen_ncombs(vg) == 3);
    cass_cond(bgen_phased(vg) == 0);

    bgen_close_genotype(vg);

    vg = bgen_open_genotype(bgen, vm[1].vaddr);

    cass_cond(bgen_nalleles(vg) == 2);
    cass_cond(bgen_missing(vg, 0) == 0);
    cass_cond(bgen_missing(vg, 1) == 0);
    cass_cond(bgen_missing(vg, 2) == 0);
    cass_cond(bgen_ploidy(vg, 0) == 1);
    cass_cond(bgen_ploidy(vg, 1) == 1);
    cass_cond(bgen_ploidy(vg, 2) == 1);
    cass_cond(bgen_min_ploidy(vg) == 1);
    cass_cond(bgen_max_ploidy(vg) == 1);
    cass_cond(bgen_ncombs(vg) == 2);
    cass_cond(bgen_phased(vg) == 1);

    bgen_close_genotype(vg);

    bgen_free_partition(vm, nvars);

    int phased[] = {0, 1, 1, 0, 1, 1, 1, 1, 0, 0};

    size_t i = 0;
    for (size_t j = 0; j < (size_t)bgen_metafile_npartitions(mf); ++j) {
        vm = bgen_read_partition(mf, j, &nvars);
        for (size_t l = 0; l < (size_t)nvars; ++l) {
            vg = bgen_open_genotype(bgen, vm[l].vaddr);
            cass_cond (bgen_phased(vg) == phased[i]);
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

                cass_cond(ploidys[jj] == bgen_ploidy(vg, j));
                cass_cond(bgen_missing(vg, j) == 0);

                for (size_t ii = 0; ii < (size_t)bgen_ncombs(vg); ++ii) {
                    cass_cond (!(*rp != *p && !(isnan(*rp) && isnan(*p))));
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

    cass_cond(bgen_close_metafile(mf) == 0);
    bgen_close(bgen);
}
