/* example.c */
#include <stdio.h>
#include <stdlib.h>

#include "bgen_reader/bgen_reader.h"

void print_genotypes(uint32_t *genotypes,
                     inti  nsamples,
                     inti  ngenotypes,
                     inti  nbits)
{
    inti i, j;
    uint32_t precision = 1;
    precision <<= nbits;
    precision -= 1;
    uint32_t sum;

    for (i = 0; i < nsamples / 50; ++i)
    {
        sum = 0;
        for (j = 0; j < ngenotypes - 1; ++j)
        {
            sum += genotypes[i * (ngenotypes - 1) + j];
            printf("%010u ", genotypes[i * (ngenotypes - 1) + j]);
        }

        printf("%010u \n", precision - sum);
    }
}

void print_genotypes_probabilities(uint32_t *genotypes,
                                   inti  nsamples,
                                   inti  ngenotypes,
                                   inti  nbits)
{
    inti i, j;
    inti precision = 1;
    precision <<= nbits;
    precision -= 1;
    double denom = precision;
    inti sum, s;

    for (i = 0; i < nsamples / 50; ++i)
    {
        sum = 0;
        for (j = 0; j < ngenotypes - 1; ++j)
        {
            s = genotypes[i * (ngenotypes - 1) + j];
            sum += s;
            printf("%.8f ", s / denom);
        }

        printf("%.8f \n", (precision - sum) / denom);
    }
}

void print_bgen(const char* filepath)
{
    printf("---- File name: %s ----\n", filepath);
    BGenFile *bgenfile = bgen_reader_open(filepath);
    byte     *id;
    inti  len;
    inti  sampleidx = 350;
    inti  variantidx = 187;
    inti  position, nalleles;

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
    inti  ploidy;
    inti  nbits;
    bgen_reader_read_genotype(bgenfile, variantidx, &genotypes, &ploidy,
                              &nalleles, &nbits);

    int64_t nsamples   = bgen_reader_nsamples(bgenfile);
    int64_t ngenotypes = bgen_reader_choose(nalleles + ploidy - 1, nalleles - 1);

    printf("Number of genotypes of variant index %llu: %lld\n",
           variantidx,
           ngenotypes);

    print_genotypes(genotypes, nsamples, ngenotypes, nbits);
    print_genotypes_probabilities(genotypes, nsamples, ngenotypes, nbits);

    free(genotypes);

    bgen_reader_close(bgenfile);

    printf("---- End ----\n\n");
}

int main()
{
    print_bgen("example.16bits.bgen");
    print_bgen("example.32bits.bgen");
    return 0;
}
