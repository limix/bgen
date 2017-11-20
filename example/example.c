/* example.c */
#include <stdio.h>
#include <stdlib.h>

#include "bgen/bgen.h"

int main()
{
    const byte filepath[] = "example.bgen";

    // Mandatory first call, preparing to read the file.
    BGenFile *bgen = bgen_open(filepath);

    inti nsamples = bgen_nsamples(bgen);

    printf("Number of samples: %lld\n",  nsamples);
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
    struct BGenVI *index;
    BGenVar *variants = bgen_read_variants(bgen, &index);

    printf("RSID of the first variant: %.*s\n",
           (int)variants[0].rsid.len,
           variants[0].rsid.str);

    printf("Number of alleles of the first variant: %lld\n",
           variants[0].nalleles);

    // Releasing memory associated with variant identifications.
    bgen_free_variants(bgen, variants);

    // Releases associated memory. Called at the end.
    bgen_close(bgen);

    // We now want the actual genotype probabilities of the second variant.
    // For performance reasons, it is done in two steps:

    // Step 1.
    BGenVG *vg = bgen_open_variant_genotype(index, 1);

    real *probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(real));


    // Step 2.
    bgen_read_variant_genotype(index, vg, probabilities);

    printf("Genotype probabilities of the second variant ");
    printf("for the first two samples:\n");
    inti i, j;

    for (j = 0; j < 2; ++j)
    {
        for (i = 0; i < bgen_ncombs(vg); ++i)
        {
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
