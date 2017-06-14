#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
int64_t bgen_reader_is_sampleids_block_present(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & (1 << 31)) >> 31;
}

BGenFile* bgen_reader_open(const char *filepath)
{
    BGenFile *bgenfile = malloc(sizeof(BGenFile));

    bgenfile->filepath = ft_strdup(filepath);

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
        bgenfile->sampleid_block = malloc(sizeof(SampleIdBlock));

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

int64_t bgen_reader_close(BGenFile *bgenfile)
{
    assert(bgenfile->file == NULL);

    free(bgenfile->filepath);
    bgenfile->filepath = NULL;

    uint64_t i;
    uint64_t nsamples = bgenfile->sampleid_block->nsamples;

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

int64_t bgen_reader_nsamples(BGenFile *bgenfile)
{
    return bgenfile->header.nsamples;
}

int64_t bgen_reader_nvariants(BGenFile *bgenfile)
{
    return bgenfile->header.nvariants;
}

int64_t bgen_reader_sampleid(BGenFile *bgenfile, uint64_t sample_idx, BYTE **id,
                             uint64_t *length)
{
    if (sample_idx >= bgenfile->header.nsamples) return FAIL;

    SampleId *sampleid = &(bgenfile->sampleid_block->sampleids[sample_idx]);

    *length = sampleid->length;
    *id     = sampleid->id;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variantid(BGenFile *bgenfile, uint64_t variant_idx, BYTE **id,
                              uint64_t *length)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *length = vib.id_length;
    *id     = vib.id;

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

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *length = vib.rsid_length;
    *rsid   = vib.rsid;

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

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *length = vib.chrom_length;
    *chrom  = vib.chrom;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t variant_idx, uint64_t *position)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *position = vib.position;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t variant_idx, uint64_t *nalleles)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *nalleles = vib.nalleles;

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

    VariantIdBlock vib;
    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    *id = vib.alleleids[allele_idx];

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_read_genotype(BGenFile *bgenfile, uint64_t variant_idx,
                                  uint32_t **ui_probs, uint64_t  *ploidy,
                                  uint64_t  *nalleles, uint64_t  *nbits)
{
    VariantIdBlock vib;

    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vib);

    if (FOPEN(bgenfile) == FAIL) return FAIL;

    fseek(bgenfile->file, vib.genotype_start, SEEK_SET);

    int64_t e = bgen_reader_read_current_genotype_block(bgenfile,
                                                        ploidy,
                                                        nalleles,
                                                        nbits,
                                                        ui_probs);

    if (e == FAIL) return FAIL;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_read_variantid_blocks(BGenFile      *bgenfile,
                                            VariantIdBlock **head_ref)
{
    if (FOPEN(bgenfile) == FAIL) return FAIL;

    // int64_t nsamples = bgen_reader_nsamples(bgenfile);

    bgen_reader_seek_variant_block(bgenfile, 0);

    VariantIdBlock *vib  = malloc(sizeof(VariantIdBlock));
    bgen_reader_read_current_variantid_block(bgenfile, vib);

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}
