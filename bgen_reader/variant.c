#include "bgen_reader.h"
#include "file.h"
#include "util.h"
#include "list.h"
#include "layout1.h"
#include "layout2.h"
#include "bgen_file.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#define FREAD bgen_reader_fread
#define FAIL EXIT_FAILURE
#define FOPEN bgen_reader_fopen
#define FCLOSE bgen_reader_fclose
#define NVARIANTS bgen_reader_nvariants

// Variant identifying data
//
// ---------------------------------------------
// | 4     | # samples (layout > 1)            |
// | 2     | # variant id length, Lid          |
// | Lid   | variant id                        |
// | 2     | variant rsid length, Lrsid        |
// | Lrsid | rsid                              |
// | 2     | chrom length, Lchr                |
// | Lchr  | chromossome                       |
// | 4     | variant position                  |
// | 2     | number of alleles, K (layout > 1) |
// | 4     | first allele length, La1          |
// | La1   | first allele                      |
// | 4     | second allele length, La2         |
// | La2   | second allele                     |
// | ...   |                                   |
// | 4     | last allele length, LaK           |
// | LaK   | last allele                       |
// ---------------------------------------------
int64_t bgen_reader_read_current_variantid_block(BGenFile     *bgenfile,
                                                 VariantBlock *vb)
{
    int64_t layout = bgen_reader_layout(bgenfile);

    if (layout == 1)
    {
        if (FREAD(bgenfile, &(vb->nsamples), 4)) return FAIL;
    }

    if (FREAD(bgenfile, &(vb->id_length), 2)) return FAIL;

    vb->id = malloc(vb->id_length);

    if (FREAD(bgenfile, vb->id, vb->id_length)) return FAIL;

    if (FREAD(bgenfile, &(vb->rsid_length), 2)) return FAIL;

    vb->rsid = malloc(vb->rsid_length);

    if (FREAD(bgenfile, vb->rsid, vb->rsid_length)) return FAIL;

    if (FREAD(bgenfile, &(vb->chrom_length), 2)) return FAIL;

    vb->chrom = malloc(vb->chrom_length);

    if (FREAD(bgenfile, vb->chrom, vb->chrom_length)) return FAIL;

    if (FREAD(bgenfile, &(vb->position), 4)) return FAIL;

    if (layout == 1) vb->nalleles = 2;
    else if (FREAD(bgenfile, &(vb->nalleles), 2)) return FAIL;

    size_t i;
    vb->alleles = malloc(vb->nalleles * sizeof(Allele));

    for (i = 0; i < vb->nalleles; ++i)
    {
        if (FREAD(bgenfile, &(vb->alleles[i].length),
                  4)) return FAIL;

        vb->alleles[i].id = malloc(vb->alleles[i].length);

        if (FREAD(bgenfile, vb->alleles[i].id,
                  vb->alleles[i].length)) return FAIL;
    }

    vb->genotype_start = ftell(bgenfile->file);

    return EXIT_SUCCESS;
}

// fseek to the file position from which the variant block can be read
int64_t bgen_reader_seek_variant_block(BGenFile *bgenfile, uint64_t variant_idx)
{
    assert(bgen_reader_layout(bgenfile) == 2);

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    int64_t compression = bgen_reader_compression(bgenfile);
    int64_t nsamples    = bgen_reader_nsamples(bgenfile);

    fseek(bgenfile->file, bgenfile->variants_start, SEEK_SET);

    VariantBlock vb;

    size_t i;

    for (i = 0; i < variant_idx; ++i)
    {
        bgen_reader_read_current_variantid_block(bgenfile, &vb);
        bgen_reader_read_genotype_layout2_skip(bgenfile, compression, nsamples);
    }
    return EXIT_SUCCESS;
}

// Variant identifying data
//
// ---------------------------------------------
// | 4     | # samples (layout > 1)            |
// | 2     | # variant id length, Lid          |
// | Lid   | variant id                        |
// | 2     | variant rsid length, Lrsid        |
// | Lrsid | rsid                              |
// | 2     | chrom length, Lchr                |
// | Lchr  | chromossome                       |
// | 4     | variant position                  |
// | 2     | number of alleles, K (layout > 1) |
// | 4     | first allele length, La1          |
// | La1   | first allele                      |
// | 4     | second allele length, La2         |
// | La2   | second allele                     |
// | ...   |                                   |
// | 4     | last allele length, LaK           |
// | LaK   | last allele                       |
// ---------------------------------------------
int64_t bgen_reader_read_variantid_block(BGenFile *bgenfile, uint64_t variant_idx,
                                         VariantBlock *vb)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    bgen_reader_seek_variant_block(bgenfile, variant_idx);

    bgen_reader_read_current_variantid_block(bgenfile, vb);

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_probabilities(BGenFile *bgenfile,
                                          uint64_t  variant_idx,
                                          uint32_t *ui_probs)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return -1;

    VariantBlock vb;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variantid(BGenFile *bgenfile, uint64_t variant_idx, BYTE **id,
                              uint64_t *length)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantBlock vb;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    *length = vb.id_length;
    *id     = vb.id;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_rsid(BGenFile *bgenfile,
                                 uint64_t  variant_idx,
                                 BYTE    **rsid,
                                 uint64_t *length)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantBlock vb;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    *length = vb.rsid_length;
    *rsid   = vb.rsid;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_chrom(BGenFile *bgenfile,
                                  uint64_t  variant_idx,
                                  BYTE    **chrom,
                                  uint64_t *length)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantBlock vb;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    *length = vb.chrom_length;
    *chrom  = vb.chrom;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t variant_idx, uint64_t *position)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantBlock vb;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    *position = vb.position;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t variant_idx, uint64_t *nalleles)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantBlock vb;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    *nalleles = vb.nalleles;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_alleleid(BGenFile *bgenfile, uint64_t variant_idx,
                                     uint64_t allele_idx, BYTE **id,
                                     uint64_t *length)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return -1;

    uint64_t nalleles;
    bgen_reader_variant_nalleles(bgenfile, variant_idx, &nalleles);

    if (allele_idx >= nalleles) return FAIL;

    VariantBlock vb;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    *id = vb.alleles[allele_idx].id;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_read_current_genotype_block(BGenFile     *bgenfile,
                                                uint32_t     **ui_probs)
{
    int64_t layout = bgen_reader_layout(bgenfile);
    assert(layout == 2);

    VariantGenotypeBlock vpb;
    int64_t compression = bgen_reader_compression(bgenfile);
    int64_t nsamples = bgen_reader_nsamples(bgenfile);

    return bgen_reader_read_genotype_layout2(bgenfile, compression, nsamples,
                                       &vpb, ui_probs);
}
