/* example.c */
#include <stdio.h>
#include <stdlib.h>

#include "bgen/bgen.h"

int main()
{
    const byte filepath[] = "example.bgen";

    // Mandatory first call, preparing to read the file.
    BGenFile *bgen = bgen_open(filepath);

    printf("Number of samples: %lld\n",  bgen_nsamples(bgen));
    printf("Number of variants: %lld\n", bgen_nvariants(bgen));

    // Reading sample identifications.
    string *sample_ids = bgen_read_samples(bgen);

    printf("First sample ID: %.*s\n", (int)sample_ids[0].len,
           sample_ids[0].str);

    // Releasing memory associated with sample identifications.
    bgen_free_samples(bgen, sample_ids);

    // Reading variant metadata (i.e., identifications, chromossomes, number of
    // alleles, etc.)
    // Notice that we also have a VariantIndexing in return, used afterwards.
    VariantIndexing *index;
    Variant *variants = bgen_read_variants(bgen, &index);

    printf("RSID of the first variant: %.*s\n",
           (int)variants[0].rsid.len,
           variants[0].rsid.str);

    printf("Number of alleles of the first variant: %lld\n",
           variants[0].nalleles);

    // Releasing memory associated with variant identifications.
    bgen_free_variants(bgen, variants);

    // // We now want the actual genotype probabilities of the second variant.
    // // For performance reasons, it is done in two steps:
    //
    // Step 1.
    VariantGenotype *vg = bgen_open_variant_genotype(index, 1);

    real *probabilities = malloc(bgen_ncombs(vg) * sizeof(real));

    //
    // // Step 2.
    // bgen_read_variant_genotype(index, vg, probabilities);
    //
    // printf("Genotype probabilities of the second variant:");
    // inti i;
    //
    // for (i = 0; i < bgen_ncombs(vg); ++i)
    // {
    //     printf("%f ", probabilities[i]);
    // }
    // printf("\n");
    //
    free(probabilities);

    //
    // Releases associated memory.
    bgen_close_variant_genotype(index, vg);

    // Releases associated memory. Called at the end.
    bgen_close(bgen);

    return 0;
}
