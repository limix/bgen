/* example.c */
#include <stdio.h>
#include <stdlib.h>

#include "bgen_reader/bgen_reader.h"

void print_genotypes(uint32_t *genotypes,
                     uint64_t  nsamples,
                     uint64_t  ngenotypes,
                     uint64_t  nbits)
{
    uint64_t i, j;

    for (i = 0; i < nsamples / 100; ++i)
    {
        for (j = 0; j < ngenotypes - 1; ++j)
        {
            printf("%d ", genotypes[i * (ngenotypes - 1) + j]);
        }
        printf("\n");
    }
}

int main()
{
    BGenFile *bgenfile = bgen_reader_open("example.1bits.bgen");
    BYTE     *id;
    uint64_t  len;
    uint64_t  sampleidx = 350;
    uint64_t  variantidx = 187;
    uint64_t  position, nalleles;

    printf("Number of samples: %lld\n",  bgen_reader_nsamples(bgenfile));
    printf("Number of variants: %lld\n", bgen_reader_nvariants(bgenfile));

    bgen_reader_sampleid(bgenfile, sampleidx, &id, &len);

    printf("Sample ID of sample index %llu: %.*s\n", sampleidx, (int)len, id);
    free(id);

    bgen_reader_variantid(bgenfile, variantidx, &id, &len);

    printf("Variant ID of variant index %llu: %.*s\n", variantidx, (int)len,
           id);
    free(id);

    bgen_reader_variant_rsid(bgenfile, variantidx, &id, &len);

    printf("Variant RSID of variant index %llu: %.*s\n", variantidx, (int)len,
           id);
    free(id);

    bgen_reader_variant_chrom(bgenfile, variantidx, &id, &len);

    printf("Chromossome of variant index %llu: %.*s\n", variantidx, (int)len,
           id);
    free(id);

    bgen_reader_variant_position(bgenfile, variantidx, &position);

    printf("Position of variant index %llu: %llu\n", variantidx, position);

    bgen_reader_variant_nalleles(bgenfile, variantidx, &nalleles);

    printf("Number of alleles of variant index %llu: %llu\n", variantidx,
           nalleles);
    free(id);

    int alleleidx;

    for (alleleidx = 0; alleleidx < 2; ++alleleidx)
    {
        bgen_reader_variant_alleleid(bgenfile, variantidx, alleleidx, &id, &len);

        printf("Allele id of allele %d of variant index %llu: %.*s\n",
               alleleidx,
               variantidx,
               (int)len,
               id);
        free(id);
    }

    uint32_t *genotypes;
    uint64_t  ploidy;
    uint64_t  nbits;
    bgen_reader_read_genotype(bgenfile, variantidx, &genotypes, &ploidy, &nbits,
                              &nalleles);

    int64_t nsamples   = bgen_reader_nsamples(bgenfile);
    int64_t ngenotypes = bgen_reader_choose(nalleles + ploidy - 1, nalleles - 1);

    printf("Number of genotypes of variant index %llu: %lld\n",
           variantidx,
           ngenotypes);

    print_genotypes(genotypes, nsamples, ngenotypes, nbits);

    free(genotypes);

    bgen_reader_close(bgenfile);
}
