/* read_probability.c */
#include "bgen.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{

    struct bgen_file *bgen = bgen_open("example.bgen");
    if (bgen == NULL) {
        printf("Failed to open it.\n");
        return 1;
    }

    struct bgen_mf *mf = bgen_open_metafile("example.bgen.metafile");

    int nvariants = 0;
    int partition_idx = 1;
    int variant_idx = 3;

    // We need `vaddr` value of the variant in order to open its genotype.
    struct bgen_vm *variants = bgen_read_partition(mf, partition_idx, &nvariants);
    long vaddr = variants[variant_idx].vaddr;
    printf("We've got vaddr %ld for variant %.*s.\n", vaddr,
           variants[variant_idx].id.len, variants[variant_idx].id.str);
    bgen_free_partition(variants, nvariants);

    // We now open the genotype and query for data.
    struct bgen_vg *vg = bgen_open_genotype(bgen, vaddr);
    int sample_idx = 23;
    printf("Ploidy for sample %d: %d.\n", sample_idx, bgen_ploidy(vg, sample_idx));
    if (bgen_phased(vg))
        printf("Genotype is phased.\n");
    else
        printf("Genotype is unphased.\n");

    // Read the genotype probabilities for each sample.
    int nsamples = bgen_nsamples(bgen);
    double *p = malloc(nsamples * bgen_ncombs(vg));
    if (bgen_read_genotype(bgen, vg, p)) {
        printf("Failure!\n");
        return 1;
    }
    printf("Genotype probabilities for sample %d:\n", sample_idx);
    for (int i = 0; i < bgen_ncombs(vg); ++i)
        printf("%f ", p[i]);
    printf("\n");

    bgen_close_genotype(vg);

    if (bgen_close_metafile(mf)) {
        printf("Failed to close it.\n");
        return 1;
    }

    bgen_close(bgen);

    return 0;
}
