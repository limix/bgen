#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgen_reader.h"
#include "bgen_file.h"
#include "variant.h"
#include "sample.h"
#include "util/mem.h"

#define FREAD bgen_reader_fread
#define FAIL EXIT_FAILURE
#define FOPEN bgen_reader_fopen
#define FCLOSE bgen_reader_fclose
#define NVARIANTS bgen_reader_nvariants

// Is sample identifier block present?
inti bgen_reader_is_sampleids_block_present(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & (1 << 31)) >> 31;
}

BGenFile* bgen_reader_open(const char *filepath)
{
    BGenFile *bgenfile = calloc(1, sizeof(BGenFile));

    bgenfile->filepath = (char *)ft_strdup((const byte *)filepath);

    if (FOPEN(bgenfile) == FAIL) return NULL;

    if (bgenfile->file == NULL) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return NULL;
    }

    uint32_t offset;

    // First four bytes (offset)
    if (bgen_reader_fread(bgenfile, &offset, 4)) return NULL;

    if (bgen_reader_read_header(bgenfile, &(bgenfile->header))) return NULL;

    if (bgenfile->header.header_length > offset)
    {
        fprintf(stderr, "Header length is larger then offset's.\n");
        FCLOSE(bgenfile);
        return NULL;
    }

    if (bgen_reader_is_sampleids_block_present(bgenfile))
    {
        bgenfile->sampleid_block = calloc(1, sizeof(SampleIdBlock));

        if (bgen_read_sampleid_block(bgenfile) == FAIL)
        {
            FCLOSE(bgenfile);
            return NULL;
        }
    }

    bgenfile->variants_start = ftell(bgenfile->file);

    if (bgenfile->variants_start == EOF)
    {
        perror("Could not find variant blocks");
        FCLOSE(bgenfile);
        return NULL;
    }

    if (FCLOSE(bgenfile) == FAIL) return NULL;

    return bgenfile;
}

inti bgen_reader_close(BGenFile *bgenfile)
{
    assert(bgenfile->file == NULL);

    free(bgenfile->filepath);
    bgenfile->filepath = NULL;

    inti i;
    inti nsamples = bgenfile->sampleid_block->nsamples;

    if (bgen_reader_is_sampleids_block_present(bgenfile))
    {
        assert(bgenfile->sampleid_block != NULL);

        for (i = 0; i < nsamples; ++i)
        {
            free(bgenfile->sampleid_block->sampleids[i].id);
        }

        free(bgenfile->sampleid_block->sampleids);
        free(bgenfile->sampleid_block);
        bgenfile->sampleid_block = NULL;
    }

    free(bgenfile);
    return EXIT_SUCCESS;
}

inti bgen_reader_nsamples(BGenFile *bgenfile)
{
    return bgenfile->header.nsamples;
}

inti bgen_reader_nvariants(BGenFile *bgenfile)
{
    return bgenfile->header.nvariants;
}

inti bgen_reader_sampleid(BGenFile *bgenfile, inti sample_idx, byte **id,
                          inti *length)
{
    if (sample_idx >= bgenfile->header.nsamples) return FAIL;

    SampleId *sampleid = &(bgenfile->sampleid_block->sampleids[sample_idx]);

    *length = sampleid->length;
    *id     = sampleid->id;

    return EXIT_SUCCESS;
}

inti bgen_reader_variantid(BGenFile *bgenfile, inti variant_idx, byte **id,
                           inti *length)
{
    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;

    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *length = vib.id_length;
    *id     = ft_strndup(vib.id, *length);

    bgen_reader_free_variantid_block(&vib);

    return EXIT_SUCCESS;
}

inti bgen_reader_variant_rsid(BGenFile *bgenfile,
                              inti      variant_idx,
                              byte    **rsid,
                              inti     *length)
{
    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *length = vib.rsid_length;
    *rsid   = ft_strndup(vib.rsid, *length);

    bgen_reader_free_variantid_block(&vib);

    return EXIT_SUCCESS;
}

inti bgen_reader_variant_chrom(BGenFile *bgenfile,
                               inti      variant_idx,
                               byte    **chrom,
                               inti     *length)
{
    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *length = vib.chrom_length;
    *chrom  = ft_strndup(vib.chrom, *length);

    bgen_reader_free_variantid_block(&vib);

    return EXIT_SUCCESS;
}

inti bgen_reader_variant_position(BGenFile *bgenfile,
                                  inti variant_idx, inti *position)
{
    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *position = vib.position;

    bgen_reader_free_variantid_block(&vib);

    return EXIT_SUCCESS;
}

inti bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                  inti variant_idx, inti *nalleles)
{
    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *nalleles = vib.nalleles;

    bgen_reader_free_variantid_block(&vib);

    return EXIT_SUCCESS;
}

inti bgen_reader_variant_alleleid(BGenFile *bgenfile, inti variant_idx,
                                  inti allele_idx, byte **id,
                                  inti *length)
{
    if (variant_idx >= NVARIANTS(bgenfile)) return -1;

    inti nalleles;
    bgen_reader_variant_nalleles(bgenfile, variant_idx, &nalleles);

    if (allele_idx >= nalleles) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *length = vib.allele_lengths[allele_idx];
    *id     = ft_strndup(vib.alleleids[allele_idx], *length);

    bgen_reader_free_variantid_block(&vib);

    return EXIT_SUCCESS;
}

inti bgen_reader_read_genotype(BGenFile *bgenfile, inti variant_idx,
                               inti **ui_probs, inti  *ploidy,
                               inti  *nalleles, inti  *nbits)
{
    VariantIdBlock vib;

    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    if (FOPEN(bgenfile) == FAIL) return FAIL;

    fseek(bgenfile->file, vib.genotype_start, SEEK_SET);

    inti e = bgen_reader_read_current_genotype_block(bgenfile,
                                                     ploidy,
                                                     nalleles,
                                                     nbits,
                                                     ui_probs);

    if (e == FAIL) return FAIL;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

inti bgen_reader_read_variantid_blocks(BGenFile        *bgenfile,
                                       VariantIdBlock **head_ref)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    inti nvariants = bgen_reader_nvariants(bgenfile);

    bgen_reader_seek_variant_block(bgenfile, 0);

    VariantIdBlock *vib;
    *head_ref = NULL;
    inti i;

    for (i = 0; i < nvariants; ++i)
    {
        VariantIdBlock *vib = malloc(sizeof(VariantIdBlock));
        bgen_reader_read_current_variantid_block(bgenfile, vib);
        vib->next = *head_ref;
        bgen_reader_genotype_skip(bgenfile);
        *head_ref = vib;
    }

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
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
