#include "bgen/bgen.h"

#include <stdio.h>
#include <string.h>

// #include <assert.h>
// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>


#define SUCCESS EXIT_SUCCESS
#define FAIL EXIT_FAILURE

// static inline int bytencmp(const byte *s1, const char *s2, inti n)
// {
//     return strncmp((const char *)s1, s2, n);
// }
//
// static int test_sampleids_block(BGenFile *bgen)
// {
//     byte *sampleid;
//     inti  sampleid_len;
//
//     if (bgen_reader_sampleid(bgen, 0, &sampleid,
//                              &sampleid_len)) return FAIL;
//
//     if (bytencmp(sampleid, "sample_001",
//                  sampleid_len) != 0) return FAIL;
//
//     if (bgen_reader_sampleid(bgen, 499, &sampleid,
//                              &sampleid_len)) return FAIL;
//
//     if (bytencmp(sampleid, "sample_500",
//                  sampleid_len) != 0) return FAIL;
//
//     if (bgen_reader_sampleid(bgen, 500, &sampleid,
//                              &sampleid_len) !=
//         FAIL) return FAIL;
//
//     return EXIT_SUCCESS;
// }
//
// static int test_variants_block(BGenFile *bgen)
// {
//     byte *varid, *var_rsid, *var_chrom, *alleleid;
//     inti  varid_len, var_rsid_len, var_chrom_len, alleleid_len;
//
//     bgen_reader_variantid(bgen, 0, &varid, &varid_len);
//
//
//     if (bytencmp(varid, "SNPID_2", varid_len) != 0) return FAIL;
//
//
//     bgen_reader_variant_rsid(bgen, 0, &var_rsid, &var_rsid_len);
//
//
//     if (bytencmp(var_rsid, "RSID_2", var_rsid_len) != 0) return FAIL;
//
//     bgen_reader_variant_chrom(bgen, 0, &var_chrom, &var_chrom_len);
//
//     if (bytencmp(var_chrom, "01", var_chrom_len) != 0) return FAIL;
//
//     inti pos;
//
//     bgen_reader_variant_position(bgen, 0, &pos);
//
//     if (pos != 2000) return FAIL;
//
//     inti nalleles;
//     bgen_reader_variant_nalleles(bgen, 0, &nalleles);
//
//     if (nalleles != 2) return FAIL;
//
//     bgen_reader_variant_alleleid(bgen, 0, 0, &alleleid, &alleleid_len);
//
//     if (bytencmp(alleleid, "A", alleleid_len) != 0) return FAIL;
//
//     bgen_reader_variant_alleleid(bgen, 0, 1, &alleleid, &alleleid_len);
//
//
//     if (bytencmp(alleleid, "G", alleleid_len) != 0) return FAIL;
//
//     return EXIT_SUCCESS;
// }
//
// int test_genotype_reading(BGenFile *bgen)
// {
//     inti nsamples, ploidy, nalleles, nbits;
//
//     nsamples = bgen_reader_nsamples(bgen);
//
//     inti *ui_probs;
//
//     // first SNP
//     bgen_reader_read_genotype(bgen, 0, &ui_probs, &ploidy, &nalleles,
//                               &nbits);
//
//     if (nbits < 1) return FAIL;
//
//     inti precision = 1;
//     precision <<= nbits;
//     precision  -= 1;
//     inti sum;
//
//     // Sample 0 (sample_001)
//     sum = ui_probs[0] + ui_probs[1];
//
//     if ((ui_probs[0] > precision - sum) || (ui_probs[1] > precision - sum))
// return FAIL;
//
//     // Sample 3 (sample_004)
//     sum = ui_probs[6] + ui_probs[7];
//
//     if ((ui_probs[6] > ui_probs[7]) || (precision - sum > ui_probs[7]))
// return FAIL;
//
//     // Sample 498 (sample_499)
//     sum = ui_probs[498 * 2] + ui_probs[498 * 2 + 1];
//
//     if (ui_probs[498 * 2] < ui_probs[498 * 2 + 1]) return FAIL;
//
//     if (ui_probs[498 * 2] < precision - sum) return FAIL;
//
//     free(ui_probs);
//
//     // second SNP
//     bgen_reader_read_genotype(bgen, 1, &ui_probs, &ploidy, &nalleles,
//                               &nbits);
//
//     if (nbits < 1) return FAIL;
//
//     precision   = 1;
//     precision <<= nbits;
//     precision  -= 1;
//
//     sum = ui_probs[0] + ui_probs[1];
//
//     if ((precision - sum < ui_probs[0]) || (precision - sum < ui_probs[1]))
// return FAIL;
//
//     sum = ui_probs[4] + ui_probs[5];
//
//     if (ui_probs[4] < precision - sum) return FAIL;
//
//     if (ui_probs[4] < ui_probs[5]) return FAIL;
//
//     sum = ui_probs[12] + ui_probs[13];
//
//     if (ui_probs[13] < ui_probs[12]) return FAIL;
//
//     if (ui_probs[13] < precision - sum) return FAIL;
//
//     free(ui_probs);
//
//     return EXIT_SUCCESS;
// }
//
// int test_variantid_blocks_reading(BGenFile *bgen)
// {
//     VariantIdBlock *head_ref = NULL;
//
//     if (bgen_reader_read_variantid_blocks(bgen, &head_ref) == FAIL)
// return FAIL;
//
//     VariantIdBlock *vib;
//     char snp_name[32];
//
//     inti i = 200;
//     inti length;
//
//     while (head_ref != NULL)
//     {
//         length = sprintf(snp_name, "SNPID_%d", (int)i);
//
//         if (strncmp((char *)snp_name, (char *)head_ref->id, length) != 0)
// return FAIL;
//
//         vib      = head_ref;
//         head_ref = head_ref->next;
//         bgen_reader_free_variantid_block(vib);
//         --i;
//     }
//
//     return EXIT_SUCCESS;
// }

int test_filepath(const byte *filepath)
{
    BGenFile *bgen;

    bgen = bgen_open(filepath);

    if (bgen == NULL) return FAIL;

    if (bgen_nsamples(bgen) != 500) return FAIL;

    if (bgen_nvariants(bgen) != 199) return FAIL;

    string *samples = bgen_read_samples(bgen);

    if (samples == NULL) return FAIL;

    inti e = strncmp("sample_001", (char *)samples[0].str, samples[0].len);

    if (e != 0) return FAIL;

    e = strncmp("sample_500", (char *)samples[499].str, samples[0].len);

    if (e != 0) return FAIL;

    // printf("%.*s\n", samples[0].len, samples[0].str);

    // if (test_sampleids_block(bgen) != EXIT_SUCCESS) return FAIL;
    //
    // if (test_variants_block(bgen) != EXIT_SUCCESS) return FAIL;
    //
    // if (test_genotype_reading(bgen) != EXIT_SUCCESS) return FAIL;
    //
    // if (test_variantid_blocks_reading(bgen) != EXIT_SUCCESS) return FAIL;
    //
    bgen_close(bgen);

    return EXIT_SUCCESS;
}

int main()
{
    if (test_filepath((byte *)"test/data/example.1bits.bgen") == FAIL) return FAIL;

    if (test_filepath((byte *)"test/data/example.32bits.bgen") == FAIL) return FAIL;

    return EXIT_SUCCESS;
}
