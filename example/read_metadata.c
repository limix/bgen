/* read_metadata.c */
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
    for (int partition = 0; partition < bgen_metafile_npartitions(mf); ++partition) {
        struct bgen_vm *variants = bgen_read_partition(mf, partition, &nvariants);
        printf("Number of variants in partition %d: %d\n", partition, nvariants);
        bgen_free_partition(variants, nvariants);
    }

    struct bgen_vm *variants = bgen_read_partition(mf, 0, &nvariants);
    printf("First variant id: %.*s\n", variants[0].id.len, variants[0].id.str);
    printf("First variant rsid: %.*s\n", variants[0].rsid.len, variants[0].id.str);
    printf("First variant chrom: %.*s\n", variants[0].chrom.len, variants[0].chrom.str);
    printf("Number of alleles: %d\n", variants[0].nalleles);
    printf("First allele: %.*s\n", variants[0].allele_ids[0].len,
           variants[0].allele_ids[0].str);
    printf("Second allele: %.*s\n", variants[0].allele_ids[0].len,
           variants[0].allele_ids[0].str);
    bgen_free_partition(variants, nvariants);

    if (bgen_close_metafile(mf)) {
        printf("Failed to close it.\n");
        return 1;
    }

    bgen_close(bgen);

    return 0;
}
