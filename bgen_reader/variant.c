#include "bgen_reader.h"
#include "file.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

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

    if (fread_check(&(vb->position), 4, f, fp)) return EXIT_FAILURE;

    if (bgen_reader_layout(bgenfile) == 1) vb->nalleles = 2;
    else if (fread_check(&(vb->nalleles), 2, f, fp)) return EXIT_FAILURE;


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

int64_t bgen_reader_variant_id(BGenFile *bgenfile, uint64_t idx, char **id,
                               uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.id_length;
    *id     = vb.id;

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

int64_t bgen_reader_variant_rsid(BGenFile *bgenfile, uint64_t idx, char **rsid,
                                 uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.rsid_length;
    *rsid   = vb.rsid;

    fclose(f);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_chrom(BGenFile *bgenfile, uint64_t idx, char **chrom,
                                  uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.chrom_length;
    *chrom  = vb.chrom;

    fclose(f);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t  idx)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return -1;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return -1;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    fclose(f);

    return vb.position;
}

int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t  idx)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return -1;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return -1;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    fclose(f);

    return vb.nalleles;
}
