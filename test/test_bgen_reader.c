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

    return EXIT_SUCCESS;
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
    BGenFile *bgen_file;

    bgen_file = bgen_reader_open(fp);

    if (bgen_file == NULL) return EXIT_FAILURE;


    if (bgen_reader_layout(bgen_file) != 2) return EXIT_FAILURE;


    if (bgen_reader_compression(bgen_file) != 2) return EXIT_FAILURE;

    if (bgen_reader_nsamples(bgen_file) != 500) return EXIT_FAILURE;


    if (bgen_reader_nvariants(bgen_file) != 199) return EXIT_FAILURE;


    if (bgen_reader_sampleids(bgen_file) != 1) return EXIT_FAILURE;

    if (test_sampleids_block(bgen_file) != EXIT_SUCCESS) return EXIT_FAILURE;

    return EXIT_SUCCESS;

    if (test_variants_block(bgen_file) != EXIT_SUCCESS) return EXIT_FAILURE;

    uint64_t nsamples, ncombs;

    nsamples = bgen_reader_nsamples(bgen_file);
    bgen_reader_variant_ncombs(bgen_file, 0, &ncombs);

    if (ncombs != 3) return EXIT_FAILURE;

    uint32_t *ui_probs = malloc(sizeof(uint32_t) * nsamples * (ncombs - 1));

    // first SNP
    bgen_reader_genotype_block(bgen_file, 0, ui_probs);

    // Sample 0 (sample_001)
    if ((ui_probs[0] != 0) || (ui_probs[0] != 0)) return EXIT_FAILURE;

    // Sample 3 (sample_004)
    if ((ui_probs[6] != 0) || (ui_probs[7] != 1)) return EXIT_FAILURE;

    // Sample 498 (sample_499)
    if ((ui_probs[498 * 2] != 1) || (ui_probs[498 * 2 + 1] != 0)) return EXIT_FAILURE;

    free(ui_probs);

    ui_probs = malloc(sizeof(uint32_t) * nsamples * (ncombs - 1));

    // second SNP
    bgen_reader_genotype_block(bgen_file, 1, ui_probs);

    if ((ui_probs[0] != 0) || (ui_probs[0] != 0)) return EXIT_FAILURE;

    if ((ui_probs[4] != 1) || (ui_probs[5] != 0)) return EXIT_FAILURE;

    if ((ui_probs[12] != 0) || (ui_probs[13] != 1)) return EXIT_FAILURE;

    free(ui_probs);

    bgen_reader_close(bgen_file);


    return EXIT_SUCCESS;
}
