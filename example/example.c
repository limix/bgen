/* example.c */
#include <stdio.h>
#include <stdlib.h>

#include "bgen.h"

int main() {
    const char filepath[] = "example.bgen";

    // Mandatory first call, preparing to read the file.
    struct bgen_file *bgen = bgen_open(filepath);

    int nsamples = bgen_nsamples(bgen);

    printf("Number of samples: %d\n", nsamples);
    printf("Number of variants: %d\n", bgen_nvariants(bgen));

    // Reading sample identifications.
    struct bgen_string *sample_ids = bgen_read_samples(bgen, 0);

    printf("First sample ID: %.*s\n", (int)sample_ids[0].len,
           sample_ids[0].str);

    // Releasing memory associated with sample identifications.
    bgen_free_samples(bgen, sample_ids);

    // Reading variant metadata (i.e., identifications, chromosomes, number of
    // alleles, etc.)
    // Notice that we also have a bgen_vi in return, used afterwards.
    struct bgen_vi *index;
    struct bgen_var *variants = bgen_read_variants_metadata(bgen, &index, 0);

    printf("RSID of the first variant: %.*s\n", (int)variants[0].rsid.len,
           variants[0].rsid.str);

    printf("Number of alleles of the first variant: %d\n",
           variants[0].nalleles);

    // Releasing memory associated with variant identifications.
    bgen_free_variants_metadata(bgen, variants);

    // Releases associated memory. Called at the end.
    bgen_close(bgen);

    // We now want the actual genotype probabilities of the second variant.
    // For performance reasons, it is done in two steps:

    // Step 1.
    struct bgen_vg *vg = bgen_open_variant_genotype(index, 1);

    double *probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(double));

    // Step 2.
    bgen_read_variant_genotype(index, vg, probabilities);

    printf("Genotype probabilities of the second variant ");
    printf("for the first two samples:\n");
    size_t i, j;

    for (j = 0; j < 2; ++j) {
        for (i = 0; i < bgen_ncombs(vg); ++i) {
            printf("  %f", probabilities[j * bgen_ncombs(vg) + i]);
        }
        printf("\n");
    }

    free(probabilities);

    // Releases associated memory when we are done reading this particular
    // variant genotype.
    bgen_close_variant_genotype(index, vg);

    // Releases associated memory when we are done reading variant
    // genotypes.
    bgen_free_index(index);

    return 0;
}
