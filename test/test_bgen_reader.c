#include "bgen_reader/bgen_reader.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define byte unsigned char
#define FAIL EXIT_FAILURE

static inline int bytencmp(const byte *s1, const char *s2, inti n)
{
    return strncmp((const char *)s1, s2, n);
}

static int test_sampleids_block(BGenFile *bgenfile)
{
    byte *sampleid;
    inti sampleid_len;

    if (bgen_reader_sampleid(bgenfile, 0, &sampleid,
                             &sampleid_len)) return FAIL;

    if (bytencmp(sampleid, "sample_001",
                 sampleid_len) != 0) return FAIL;

    if (bgen_reader_sampleid(bgenfile, 499, &sampleid,
                             &sampleid_len)) return FAIL;

    if (bytencmp(sampleid, "sample_500",
                 sampleid_len) != 0) return FAIL;

    if (bgen_reader_sampleid(bgenfile, 500, &sampleid,
                             &sampleid_len) !=
        FAIL) return FAIL;

    return EXIT_SUCCESS;
}

static int test_variants_block(BGenFile *bgenfile)
{
    byte *varid, *var_rsid, *var_chrom, *alleleid;
    inti varid_len, var_rsid_len, var_chrom_len, alleleid_len;

    bgen_reader_variantid(bgenfile, 0, &varid, &varid_len);


    if (bytencmp(varid, "SNPID_2", varid_len) != 0) return FAIL;


    bgen_reader_variant_rsid(bgenfile, 0, &var_rsid, &var_rsid_len);


    if (bytencmp(var_rsid, "RSID_2", var_rsid_len) != 0) return FAIL;

    bgen_reader_variant_chrom(bgenfile, 0, &var_chrom, &var_chrom_len);

    if (bytencmp(var_chrom, "01", var_chrom_len) != 0) return FAIL;

    inti pos;

    bgen_reader_variant_position(bgenfile, 0, &pos);

    if (pos != 2000) return FAIL;

    inti nalleles;
    bgen_reader_variant_nalleles(bgenfile, 0, &nalleles);

    if (nalleles != 2) return FAIL;

    bgen_reader_variant_alleleid(bgenfile, 0, 0, &alleleid, &alleleid_len);

    if (bytencmp(alleleid, "A", alleleid_len) != 0) return FAIL;

    bgen_reader_variant_alleleid(bgenfile, 0, 1, &alleleid, &alleleid_len);


    if (bytencmp(alleleid, "G", alleleid_len) != 0) return FAIL;

    return EXIT_SUCCESS;
}

int test_genotype_reading(BGenFile *bgenfile)
{
    inti nsamples, ploidy, nalleles, nbits;

    nsamples = bgen_reader_nsamples(bgenfile);

    inti *ui_probs;

    // first SNP
    bgen_reader_read_genotype(bgenfile, 0, &ui_probs, &ploidy, &nalleles,
                              &nbits);

    if (nbits != 1) return FAIL;

    // Sample 0 (sample_001)
    if ((ui_probs[0] != 0) || (ui_probs[0] != 0)) return FAIL;

    // Sample 3 (sample_004)
    if ((ui_probs[6] != 0) || (ui_probs[7] != 1)) return FAIL;

    // Sample 498 (sample_499)
    if ((ui_probs[498 * 2] != 1) ||
        (ui_probs[498 * 2 + 1] != 0)) return FAIL;

    free(ui_probs);

    // second SNP
    bgen_reader_read_genotype(bgenfile, 1, &ui_probs, &ploidy, &nalleles,
                              &nbits);

    if (nbits != 1) return FAIL;

    if ((ui_probs[0] != 0) || (ui_probs[0] != 0)) return FAIL;

    if ((ui_probs[4] != 1) || (ui_probs[5] != 0)) return FAIL;

    if ((ui_probs[12] != 0) || (ui_probs[13] != 1)) return FAIL;

    free(ui_probs);

    return EXIT_SUCCESS;
}

int test_variantid_blocks_reading(BGenFile *bgenfile)
{
    VariantIdBlock *head_ref = NULL;
    if (bgen_reader_read_variantid_blocks(bgenfile, &head_ref) == FAIL) return FAIL;
    return EXIT_SUCCESS;
}

int main()
{
    char *fp = "test/data/example.1bits.bgen";
    BGenFile *bgenfile;

    bgenfile = bgen_reader_open(fp);


    if (bgenfile == NULL) return FAIL;


    if (bgen_reader_nsamples(bgenfile) != 500) return FAIL;


    if (bgen_reader_nvariants(bgenfile) != 199) return FAIL;


    if (test_sampleids_block(bgenfile) != EXIT_SUCCESS) return FAIL;


    if (test_variants_block(bgenfile) != EXIT_SUCCESS) return FAIL;

    if (test_genotype_reading(bgenfile) != EXIT_SUCCESS) return FAIL;

    if (test_variantid_blocks_reading(bgenfile) != EXIT_SUCCESS) return FAIL;

    bgen_reader_close(bgenfile);

    return EXIT_SUCCESS;
}
