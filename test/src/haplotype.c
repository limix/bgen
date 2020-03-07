#include "bgen/bgen.h"
#include "cass.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_haplotype(void);

int main(void)
{
    test_haplotype();
    return cass_status();
}

void test_haplotype(void)
{
    size_t i, j, ii, jj;
    const char filename[] = "data/haplotypes.bgen";
    struct bgen_file* bgen;
    int nsamples, nvariants;

    cass_cond((bgen = bgen_open(filename)) != NULL);
    cass_cond((nsamples = bgen_nsamples(bgen)) == 4);
    cass_cond((nvariants = bgen_nvariants(bgen)) == 4);

    struct bgen_str* sample_ids = bgen_read_samples(bgen, 0);

    cass_cond(strncmp("sample_0", sample_ids[0].str, sample_ids[0].len) == 0);

    bgen_free_samples(bgen, sample_ids);

    struct bgen_vi* index;
    struct bgen_mf* mf = bgen_create_metafile(bgen, "complex.23bits.bgen.metadata.1", 1, 0);

    cass_cond(mf != NULL);
    cass_cond(bgen_metafile_npartitions(mf) == 1);

    struct bgen_vm* vm = bgen_read_partition(mf, 0, &nvariants);

    cass_cond(vm != NULL);
    cass_cond(nvariants == 4);

    cass_cond(strncmp("RS1", vm[0].rsid.str, vm[0].rsid.len) == 0);
    cass_cond(vm[0].nalleles == 2);

    for (i = 0; i < (size_t)nvariants; ++i) {
        cass_cond(strncmp("A", vm[i].allele_ids[0].str, vm[i].allele_ids[0].len) == 0);
        cass_cond(strncmp("G", vm[i].allele_ids[1].str, vm[i].allele_ids[1].len) == 0);
    }

    struct bgen_vg* vg = bgen_open_genotype(bgen, vm[1].vaddr);

    cass_cond(bgen_ncombs(vg) == 4);
    cass_cond(bgen_ploidy(vg, 0) == 2);
    cass_cond(bgen_min_ploidy(vg) == 2);
    cass_cond(bgen_max_ploidy(vg) == 2);
    cass_cond(bgen_nalleles(vg) == 2);
    cass_cond(bgen_phased(vg) == 1);

    bgen_close_genotype(vg);

    double real_probs[] = {1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                           1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
                           0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0,
                           1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
                           1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0};

    jj = 0;
    for (i = 0; i < (size_t)nvariants; ++i) {
        vg = bgen_open_genotype(bgen, vm[i].vaddr);

        double* probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(double));

        cass_cond(bgen_read_genotype(bgen, vg, probabilities) == 0);

        for (ii = 0; ii < (size_t)nsamples; ++ii) {
            for (j = 0; j < (size_t)bgen_ncombs(vg); ++j) {
                cass_cond(probabilities[ii * bgen_ncombs(vg) + j] == real_probs[jj]);
                jj++;
            }
        }
        bgen_close_genotype(vg);
        free(probabilities);
    }

    bgen_free_partition(vm, nvariants);
    cass_cond(bgen_close_metafile(mf) == 0);
    bgen_close(bgen);
}