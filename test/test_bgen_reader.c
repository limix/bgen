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

static int test_sampleids_block(BGenFile *bgen_file)
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

static int test_variants_block(BGenFile *bgen_file)
{
    BYTE *varid, *var_rsid, *var_chrom, *alleleid;
    uint64_t varid_len, var_rsid_len, var_chrom_len, alleleid_len;

    bgen_reader_variantid(bgen_file, 0, &varid, &varid_len);


    if (bytencmp(varid, "SNPID_2", varid_len) != 0) return EXIT_FAILURE;


    bgen_reader_variant_rsid(bgen_file, 0, &var_rsid, &var_rsid_len);


    if (bytencmp(var_rsid, "RSID_2", var_rsid_len) != 0) return EXIT_FAILURE;

    bgen_reader_variant_chrom(bgen_file, 0, &var_chrom, &var_chrom_len);

    if (bytencmp(var_chrom, "01", var_chrom_len) != 0) return EXIT_FAILURE;

    uint64_t pos;

    bgen_reader_variant_position(bgen_file, 0, &pos);

    if (pos != 2000) return EXIT_FAILURE;

    uint64_t nalleles;
    bgen_reader_variant_nalleles(bgen_file, 0, &nalleles);

    if (nalleles != 2) return EXIT_FAILURE;

    bgen_reader_variant_alleleid(bgen_file, 0, 0, &alleleid, &alleleid_len);

    if (bytencmp(alleleid, "A", alleleid_len) != 0) return EXIT_FAILURE;

    bgen_reader_variant_alleleid(bgen_file, 0, 1, &alleleid, &alleleid_len);


    if (bytencmp(alleleid, "G", alleleid_len) != 0) return EXIT_FAILURE;

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


    if (test_sampleids_block(&bgen_file) != EXIT_SUCCESS) return EXIT_FAILURE;

    if (test_variants_block(&bgen_file) != EXIT_SUCCESS) return EXIT_FAILURE;


    uint64_t nsamples, ncombs;

    nsamples = bgen_reader_nsamples(&bgen_file);
    bgen_reader_variant_ncombs(&bgen_file, 0, &ncombs);

    if (ncombs != 3) return EXIT_FAILURE;

    uint32_t *ui_probs = malloc(sizeof(uint32_t) * nsamples * (ncombs - 1));

    VariantBlock vb;

    // first SNP
    bgen_reader_genotype_block(&bgen_file, 0, &vb, ui_probs);

    // Sample 0 (sample_001)
    if (ui_probs[0] != 0 || ui_probs[0] != 0) return EXIT_FAILURE;
    // Sample 3 (sample_004)
    if (ui_probs[6] != 0 || ui_probs[7] != 1) return EXIT_FAILURE;
    // Sample 498 (sample_499)
    if (ui_probs[498 * 2] != 1 || ui_probs[498 * 2 + 1] != 0) return EXIT_FAILURE;

    free(ui_probs);

    ui_probs = malloc(sizeof(uint32_t) * nsamples * (ncombs - 1));

    // second SNP
    bgen_reader_genotype_block(&bgen_file, 1, &vb, ui_probs);

    if (ui_probs[0] != 0 || ui_probs[0] != 0) return EXIT_FAILURE;
    if (ui_probs[4] != 1 || ui_probs[5] != 0) return EXIT_FAILURE;

    free(ui_probs);


    // // Sample 498 (sample_499)
    // printf("ui_probs: %u %u\n", up[498 * 2], up[498 * 2 + 1]);

    // printf("ui_probs: %u %u\n", ui_probs[0], ui_probs[1]);
    // printf("ui_probs: %u %u\n", ui_probs[2], ui_probs[3]);
    // printf("ui_probs: %u %u\n", ui_probs[4], ui_probs[5]);
    // printf("ui_probs: %u %u\n", ui_probs[6], ui_probs[7]);
    // printf("ui_probs: %u %u\n", ui_probs[8], ui_probs[9]);
    // printf("ui_probs: %u %u\n", ui_probs[10], ui_probs[11]);

    // In [15]: d['genotype'].iloc[3:6, :5]
    // Out[15]:
    //     snp_id   rs_id   pos alleleA alleleB
    //
    // 1  SNPID_5  RSID_5  5000       A       G
    // 1  SNPID_6  RSID_6  6000       A       G
    // 1  SNPID_7  RSID_7  7000       A       G

    // In [21]: d['genotype'].iloc[0:2, 5:20]
    // Out[21]:
    //   sample_001                     sample_002                     sample_003  \
    //           AA        AB        BB         AA        AB        BB         AA
    // 1   0.000000  0.000000  0.000000   0.027801  0.008636  0.963531   0.017365
    // 1   0.005066  0.002075  0.992859   0.009644  0.002686  0.987671   0.979706
    //
    //                       sample_004                     sample_005            \
    //          AB        BB         AA        AB        BB         AA        AB
    // 1  0.049683  0.932922   0.024872  0.932831  0.042297   0.034607  0.019196
    // 1  0.019470  0.000824   0.981934  0.005310  0.012787   0.007202  0.984406
    //
    //
    //          BB
    // 1  0.946198
    // 1  0.008423

    bgen_reader_close(&bgen_file);



    return EXIT_SUCCESS;
}
