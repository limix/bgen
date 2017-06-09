#include "bgen_reader/bgen_reader.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int bytencmp(const BYTE *s1, const char *s2, size_t n)
{
    return strncmp((const char *)s1, s2, n);
}

static int test_sampleids(BGenFile *bgen_file)
{
    BYTE *sampleid;
    uint64_t sampleid_len;

    if (bgen_reader_sampleid(bgen_file, 0, &sampleid,
                             &sampleid_len)) return EXIT_FAILURE;

    if (bytencmp(sampleid, "sample_001",
                 sampleid_len) != 0) return EXIT_FAILURE;

    if (bgen_reader_sampleid(bgen_file, 499, &sampleid,
                             &sampleid_len)) return EXIT_FAILURE;

    if (bytencmp(sampleid, "sample_500",
                 sampleid_len) != 0) return EXIT_FAILURE;

    if (bgen_reader_sampleid(bgen_file, 500, &sampleid,
                             &sampleid_len) !=
        EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int main()
{
    char *fp = "test/data/example.1bits.bgen";
    BGenFile bgen_file;

    if (bgen_reader_open(&bgen_file, fp) != EXIT_SUCCESS) return EXIT_FAILURE;

    if (bgen_reader_layout(&bgen_file) != 2) return EXIT_FAILURE;


    if (bgen_reader_compression(&bgen_file) != 2) return EXIT_FAILURE;

    if (bgen_reader_nsamples(&bgen_file) != 500) return EXIT_FAILURE;


    if (bgen_reader_nvariants(&bgen_file) != 199) return EXIT_FAILURE;


    if (bgen_reader_sampleids(&bgen_file) != 1) return EXIT_FAILURE;


    if (test_sampleids(&bgen_file) != EXIT_SUCCESS) return EXIT_FAILURE;


    BYTE *sampleid, *varid, *var_rsid, *var_chrom, *alleleid;
    uint64_t sampleid_len, varid_len, var_rsid_len, var_chrom_len,
             alleleid_len;
    uint64_t idx;

    idx = 0;


    bgen_reader_variantid(&bgen_file, idx, &varid, &varid_len);


    printf("variant id: %s\n",       varid);
    printf("variant id len: %llu\n", varid_len);

    return EXIT_SUCCESS;


    if (bytencmp(varid, "SNPID_2", varid_len) != 0) return EXIT_FAILURE;


    bgen_reader_variant_rsid(&bgen_file, idx, &var_rsid, &var_rsid_len);


    if (bytencmp(var_rsid, "RSID_2", var_rsid_len) != 0) return EXIT_FAILURE;

    bgen_reader_variant_chrom(&bgen_file, idx, &var_chrom, &var_chrom_len);

    if (bytencmp(var_chrom, "01", var_chrom_len) != 0) return EXIT_FAILURE;

    if (bgen_reader_variant_position(&bgen_file, idx) != 2000) return EXIT_FAILURE;

    if (bgen_reader_variant_nalleles(&bgen_file, idx) != 2) return EXIT_FAILURE;

    bgen_reader_variant_alleleid(&bgen_file, idx, 0, &alleleid, &alleleid_len);

    if (bytencmp(alleleid, "A", alleleid_len) != 0) return EXIT_FAILURE;

    bgen_reader_variant_alleleid(&bgen_file, idx, 1, &alleleid, &alleleid_len);

    if (bytencmp(alleleid, "G", alleleid_len) != 0) return EXIT_FAILURE;

    idx = 0;
    VariantBlock vb;

    bgen_reader_genotype_block(&bgen_file, idx, &vb);

    bgen_reader_close(&bgen_file);

    return EXIT_SUCCESS;
}
