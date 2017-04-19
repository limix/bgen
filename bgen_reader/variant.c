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

    // printf("ftell: %ld\n", ftell(f));

    if (bgen_reader_layout(bgenfile) == 0) vb->nalleles = 2;
    else if (fread_check(&(vb->nalleles), 2, f, fp)) return EXIT_FAILURE;

    size_t i;
    vb->alleles = malloc(vb->nalleles * sizeof(Allele));

    for (i = 0; i < vb->nalleles; ++i)
    {
        if (fread_check(&(vb->alleles[i].length), 4, f, fp)) return EXIT_FAILURE;

        vb->alleles[i].id = malloc(vb->alleles[i].length);

        if (fread_check(vb->alleles[i].id, vb->alleles[i].length, f, fp)) return EXIT_FAILURE;
    }

    fclose(f);

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

int64_t bgen_reader_variant_allele_id(BGenFile *bgenfile, uint64_t idx0,
                                      uint64_t idx1, char **id,
                                      uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx0 >= bgen_reader_nvariants(bgenfile)) return -1;

    if (idx1 >= bgen_reader_variant_nalleles(bgenfile, idx0)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx0, &vb);

    *id = vb.alleles[idx1].id;

    fclose(f);

    return EXIT_SUCCESS;
}
