#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "variant.h"
#include "bgen_file.h"
#include "layout1.h"
#include "layout2.h"

#define FREAD bgen_reader_fread
#define FAIL EXIT_FAILURE
#define FOPEN bgen_reader_fopen
#define FCLOSE bgen_reader_fclose

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

    fseek(bgenfile->file, bgenfile->variants_start, SEEK_SET);

    VariantBlock vb;

    size_t i;

    for (i = 0; i < variant_idx; ++i)
    {
        bgen_reader_read_current_variantid_block(bgenfile, &vb);
        bgen_reader_read_genotype_layout2_skip(bgenfile);
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
int64_t bgen_reader_read_variantid_block(BGenFile     *bgenfile,
                                         uint64_t      variant_idx,
                                         VariantBlock *vb)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    bgen_reader_seek_variant_block(bgenfile, variant_idx);

    bgen_reader_read_current_variantid_block(bgenfile, vb);

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_read_current_genotype_block(BGenFile  *bgenfile,
                                                uint64_t  *ploidy,
                                                uint64_t  *nalleles,
                                                uint32_t **ui_probs)
{
    int64_t layout = bgen_reader_layout(bgenfile);

    assert(layout == 2);

    VariantGenotypeBlock vpb;
    int64_t compression = bgen_reader_compression(bgenfile);

    int64_t e = bgen_reader_read_genotype_layout2(bgenfile, &vpb);
    if (e == FAIL) return FAIL;

    *ploidy = vpb.max_ploidy;
    *nalleles = vpb.nalleles;
    *ui_probs = vpb.genotypes;

    return EXIT_SUCCESS;
}
