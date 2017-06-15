#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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
inti bgen_reader_read_current_variantid_block(BGenFile       *bgenfile,
                                              VariantIdBlock *vib)
{
    memset(vib, 0, sizeof(VariantIdBlock));
    inti layout = bgen_reader_layout(bgenfile);

    if (layout == 1)
    {
        if (FREAD(bgenfile, &(vib->nsamples), 4)) return FAIL;
    }

    if (FREAD(bgenfile, &(vib->id_length), 2)) return FAIL;

    vib->id = malloc(vib->id_length);

    if (FREAD(bgenfile, vib->id, vib->id_length)) return FAIL;

    if (FREAD(bgenfile, &(vib->rsid_length), 2)) return FAIL;

    vib->rsid = malloc(vib->rsid_length);

    if (FREAD(bgenfile, vib->rsid, vib->rsid_length)) return FAIL;

    if (FREAD(bgenfile, &(vib->chrom_length), 2)) return FAIL;

    vib->chrom = malloc(vib->chrom_length);

    if (FREAD(bgenfile, vib->chrom, vib->chrom_length)) return FAIL;

    if (FREAD(bgenfile, &(vib->position), 4)) return FAIL;

    if (layout == 1) vib->nalleles = 2;
    else if (FREAD(bgenfile, &(vib->nalleles), 2)) return FAIL;

    vib->allele_lengths = calloc(vib->nalleles, sizeof(inti));
    vib->alleleids      = malloc(vib->nalleles * sizeof(unsigned char *));

    inti i;

    for (i = 0; i < vib->nalleles; ++i)
    {
        if (FREAD(bgenfile, vib->allele_lengths + i, 4)) return FAIL;

        vib->alleleids[i] = malloc(vib->allele_lengths[i]);

        if (FREAD(bgenfile, vib->alleleids[i],
                  vib->allele_lengths[i])) return FAIL;
    }

    vib->genotype_start = ftell(bgenfile->file);

    return EXIT_SUCCESS;
}

// fseek to the file position from which the variant block can be read
inti bgen_reader_seek_variant_block(BGenFile *bgenfile, inti variant_idx)
{
    assert(bgen_reader_layout(bgenfile) == 2);

    fseek(bgenfile->file, bgenfile->variants_start, SEEK_SET);

    VariantIdBlock vib;

    inti i;

    for (i = 0; i < variant_idx; ++i)
    {
        bgen_reader_read_current_variantid_block(bgenfile, &vib);
        bgen_reader_layout2_genotype_skip(bgenfile);
        bgen_reader_free_variantid_block(&vib);
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
inti bgen_reader_read_variantid_block(BGenFile       *bgenfile,
                                      inti            variant_idx,
                                      VariantIdBlock *vib)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    bgen_reader_seek_variant_block(bgenfile, variant_idx);

    bgen_reader_read_current_variantid_block(bgenfile, vib);

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

inti bgen_reader_read_current_genotype_block(BGenFile *bgenfile,
                                             inti     *ploidy,
                                             inti     *nalleles,
                                             inti     *nbits,
                                             inti    **ui_probs)
{
    inti layout = bgen_reader_layout(bgenfile);

    assert(layout == 2);

    VariantGenotypeBlock vpb;
    inti compression = bgen_reader_compression(bgenfile);

    inti e = bgen_reader_read_genotype_layout2(bgenfile, &vpb);

    if (e == FAIL) return FAIL;

    *ploidy   = vpb.max_ploidy;
    *nalleles = vpb.nalleles;
    *nbits    = vpb.nbits;
    *ui_probs = vpb.genotypes;

    return EXIT_SUCCESS;
}

inti bgen_reader_genotype_skip(BGenFile *bgenfile)
{
    return bgen_reader_layout2_genotype_skip(bgenfile);
}

inti bgen_reader_free_variantid_block(VariantIdBlock *vib)
{
    free(vib->id);
    free(vib->rsid);
    free(vib->chrom);
    free(vib->allele_lengths);
    inti i;

    for (i = 0; i < vib->nalleles; ++i)
    {
        free(vib->alleleids[i]);
    }
    free(vib->alleleids);

    vib->next = NULL;

    return EXIT_SUCCESS;
}
