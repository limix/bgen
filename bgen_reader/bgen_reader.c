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

BGenFile* bgen_reader_open(char *filepath)
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

    if (bgen_reader_sampleids(bgenfile))
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

    assert(bgenfile->sampleid_block != NULL);
    free(bgenfile->sampleid_block);
    bgenfile->sampleid_block = NULL;

    free(bgenfile);
    return EXIT_SUCCESS;
}

// Is sample identifier block present?
int64_t bgen_reader_sampleids(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & (1 << 31)) >> 31;
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

int64_t bgen_reader_read_genotype(BGenFile *bgenfile, uint64_t variant_idx,
                                  uint32_t **ui_probs)
{
    VariantBlock vb;

    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    if (FOPEN(bgenfile) == FAIL) return FAIL;

    fseek(bgenfile->file, vb.genotype_start, SEEK_SET);

    int64_t e = bgen_reader_read_current_genotype_block(bgenfile, ui_probs);

    if (e == FAIL) return FAIL;

    if (FCLOSE(bgenfile) == FAIL) return FAIL;

    return EXIT_SUCCESS;
}
