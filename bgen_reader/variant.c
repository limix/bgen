#include "variant.h"

int64_t bgen_reader_variant_block(BGenFile *bgenfile, uint64_t idx,
                                  VariantBlock *vb)
{
    char *fp = bgenfile->filepath;
    FILE *f  = fopen(fp, "rb");

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    fseek(f, bgenfile->variants_start, SEEK_SET);

    if (bgen_reader_layout(bgenfile) == 0)
    {
        if (fread_check(&(vb->nsamples), 4, f, fp)) return EXIT_FAILURE;
    }

    if (fread_check(&(vb->id_length), 2, f, fp)) return EXIT_FAILURE;

    vb->id = malloc(vb->id_length);

    if (fread_check(vb->id, vb->id_length, f, fp)) return EXIT_FAILURE;

    if (fread_check(&(vb->rsid_length), 2, f, fp)) return EXIT_FAILURE;

    vb->rsid = malloc(vb->rsid_length);

    if (fread_check(vb->rsid, vb->rsid_length, f, fp)) return EXIT_FAILURE;

    if (fread_check(&(vb->chrom_length), 2, f, fp)) return EXIT_FAILURE;

    vb->chrom = malloc(vb->chrom_length);

    if (fread_check(vb->chrom, vb->chrom_length, f, fp)) return EXIT_FAILURE;

    fclose(f);

    // typedef struct
    // {
    //     uint32_t  nsamples;
    //     uint16_t  id_length;
    //     char    *id;
    //     uint16_t  rsid_length;
    //     char    *rsid;
    //     uint16_t  chrom_length;
    //     char    *chrom;
    //     uint32_t position;
    //     uint16_t  nalleles;
    //     Allele *alleles;
    // } VariantBlock;

    // VariantId *variantid = &(bgenfile->variantid_block.sampleids[idx]);
    //
    // *length = variantid->length;
    // *id     = variantid->id;

    return EXIT_SUCCESS;
}
