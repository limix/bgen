#include "bgen_reader.h"
#include "file.h"
#include "util.h"
#include "list.h"
#include "layout1.h"
#include "layout2.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

int64_t bgen_reader_variant_block_parse(BGenFile *bgenfile, VariantBlock *vb)
{
    assert(bgen_reader_layout(bgenfile) != 0);

    if (bgen_reader_layout(bgenfile) == 1)
    {
        if (fread_check(bgenfile, &(vb->nsamples), 4)) return EXIT_FAILURE;
    }

    if (fread_check(bgenfile, &(vb->id_length), 2)) return EXIT_FAILURE;

    vb->id = malloc(vb->id_length);

    if (fread_check(bgenfile, vb->id, vb->id_length)) return EXIT_FAILURE;

    if (fread_check(bgenfile, &(vb->rsid_length), 2)) return EXIT_FAILURE;

    vb->rsid = malloc(vb->rsid_length);

    if (fread_check(bgenfile, vb->rsid, vb->rsid_length)) return EXIT_FAILURE;

    if (fread_check(bgenfile, &(vb->chrom_length), 2)) return EXIT_FAILURE;

    vb->chrom = malloc(vb->chrom_length);

    if (fread_check(bgenfile, vb->chrom, vb->chrom_length)) return EXIT_FAILURE;

    if (fread_check(bgenfile, &(vb->position), 4)) return EXIT_FAILURE;

    if (bgen_reader_layout(bgenfile) == 1) vb->nalleles = 2;
    else if (fread_check(bgenfile, &(vb->nalleles), 2)) return EXIT_FAILURE;

    size_t i;
    vb->alleles = malloc(vb->nalleles * sizeof(Allele));

    for (i = 0; i < vb->nalleles; ++i)
    {
        if (fread_check(bgenfile, &(vb->alleles[i].length), 4)) return EXIT_FAILURE;

        vb->alleles[i].id = malloc(vb->alleles[i].length);

        if (fread_check(bgenfile, vb->alleles[i].id, vb->alleles[i].length)) return EXIT_FAILURE;
    }

    vb->genotype_start = ftell(bgenfile->file);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_fseek(BGenFile *bgenfile, uint64_t variant_idx)
{
    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    fseek(bgenfile->file, bgenfile->variants_start, SEEK_SET);

    VariantBlock vb;

    size_t i;

    for (i = 0; i < variant_idx; ++i)
    {
        bgen_reader_variant_block_parse(bgenfile, &vb);

        int64_t layout = bgen_reader_layout(bgenfile);
        assert(layout == 2);

        bgen_genotype_block_layout2_skip(bgenfile,
                                         bgen_reader_compression(bgenfile),
                                         bgen_reader_nsamples(bgenfile));
    }
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
int64_t bgen_reader_variant_block(BGenFile *bgenfile, uint64_t variant_idx,
                                  VariantBlock *vb)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    bgen_reader_variant_fseek(bgenfile, variant_idx);

    // fseek(bgenfile->file, bgenfile->variants_start, SEEK_SET);

    bgen_reader_variant_block_parse(bgenfile, vb);

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_genotype_block(BGenFile *bgenfile, uint64_t variant_idx,
                                   VariantBlock *vb, uint32_t *ui_probs)
{
    bgen_reader_variant_block(bgenfile, variant_idx, vb);

    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    printf("vb->genotype_start: %ld\n", vb->genotype_start);

    fseek(bgenfile->file, vb->genotype_start, SEEK_SET);

    int64_t layout = bgen_reader_layout(bgenfile);

    if (layout == 1)
    {
        bgen_genotype_block_layout1(bgenfile, bgen_reader_compression(bgenfile),
                                    bgen_reader_nsamples(bgenfile),
                                    vb, ui_probs);
    } else if (layout == 2) {
        bgen_genotype_block_layout2(bgenfile, bgen_reader_compression(bgenfile),
                                    bgen_reader_nsamples(bgenfile),
                                    vb, ui_probs);
    }

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variantid(BGenFile *bgenfile, uint64_t variant_idx, BYTE **id,
                              uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, variant_idx, &vb);

    *length = vb.id_length;
    *id     = vb.id;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_rsid(BGenFile *bgenfile, uint64_t variant_idx, BYTE **rsid,
                                 uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, variant_idx, &vb);

    *length = vb.rsid_length;
    *rsid   = vb.rsid;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_chrom(BGenFile *bgenfile, uint64_t variant_idx, BYTE **chrom,
                                  uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, variant_idx, &vb);

    *length = vb.chrom_length;
    *chrom  = vb.chrom;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t variant_idx, uint64_t *position)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, variant_idx, &vb);

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    *position = vb.position;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t variant_idx, uint64_t *nalleles)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, variant_idx, &vb);

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    *nalleles = vb.nalleles;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_ncombs(BGenFile *bgenfile,
                                   uint64_t  variant_idx,
                                   uint64_t *ncombs)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_genotype_block(bgenfile, variant_idx, &vb, NULL);

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    *ncombs = choose(vb.vpb->nalleles + vb.vpb->max_ploidy - 1, vb.vpb->nalleles - 1);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_alleleid(BGenFile *bgenfile, uint64_t variant_idx,
                                     uint64_t allele_idx, BYTE **id,
                                     uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return -1;

    uint64_t nalleles;

    bgen_reader_variant_nalleles(bgenfile, variant_idx, &nalleles);

    if (allele_idx >= nalleles) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, variant_idx, &vb);

    *id = vb.alleles[allele_idx].id;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_probabilities(BGenFile *bgenfile,
                                          uint64_t  variant_idx,
                                          uint32_t *ui_probs)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (variant_idx >= bgen_reader_nvariants(bgenfile)) return -1;

    VariantBlock vb;

    bgen_reader_genotype_block(bgenfile, variant_idx, &vb, ui_probs);

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
