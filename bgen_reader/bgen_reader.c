#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bgen_reader.h"
#include "file.h"
#include "util.h"


int64_t read_header(Header *header, FILE *restrict f, char *filepath)
{
    if (fread_check(&(header->header_length), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(header->nvariants), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(header->nsamples), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(header->magic_number), 4, f, filepath)) return EXIT_FAILURE;

    fseek(f, (header->header_length) - 20, SEEK_CUR);

    if (fread_check(&(header->flags), 4, f, filepath)) return EXIT_FAILURE;

    printf("header_length: %u\n", (header->header_length));
    printf("nvariants: %u\n",     (header->nvariants));
    printf("nsamples: %u\n",      (header->nsamples));

    return 0;
}

int read_sample_identifier_block(SampleIdBlock *block,
                                 FILE *restrict f,
                                 char          *filepath)
{
    if (fread_check(&(block->length), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(block->nsamples), 4, f, filepath)) return EXIT_FAILURE;

    block->sampleids = malloc(block->nsamples * sizeof(SampleId));

    assert(sizeof(char) == 1);

    for (size_t i = 0; i < block->nsamples; i++)
    {
        uint16_t *length = &(block->sampleids[i].length);

        if (fread_check(length, 2, f, filepath)) return EXIT_FAILURE;


        block->sampleids[i].id =
            malloc(block->sampleids[i].length * sizeof(char));

        if (fread_check(block->sampleids[i].id, block->sampleids[i].length, f,
                        filepath)) return EXIT_FAILURE;
    }
    return 0;
}

int64_t bgen_reader_read(BGenFile *bgenfile, char *filepath)
{
    FILE *f = fopen(filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", filepath);
        return EXIT_FAILURE;
    }

    bgenfile->filepath = ft_strdup(filepath);

    uint32_t offset;

    if (fread_check(&offset, 4, f, filepath)) return EXIT_FAILURE;

    printf("offset: %u\n", offset);

    if (read_header(&(bgenfile->header), f, filepath)) return EXIT_FAILURE;

    SampleIdBlock sampleid_block;

    if (bgen_reader_sample_identifiers(bgenfile))
        if (read_sample_identifier_block(&(bgenfile->sampleid_block), f,
                                         filepath)) return EXIT_FAILURE;

    bgenfile->variants_start = ftell(f);
    printf("variants_start %ld\n", bgenfile->variants_start);

    if (bgenfile->variants_start == EOF)
    {
        perror("Could not find variant blocks.");
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}

int64_t bgen_reader_layout(BGenFile *bgenfile)
{
    return bgenfile->header.flags & 3;
}

int64_t bgen_reader_sample_identifiers(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & (1 << 31)) >> 31;
}

int64_t bgen_reader_compression(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & 60) >> 2;
}

int64_t bgen_reader_nsamples(BGenFile *bgenfile)
{
    return bgenfile->header.nsamples;
}

int64_t bgen_reader_nvariants(BGenFile *bgenfile)
{
    return bgenfile->header.nvariants;
}

int64_t bgen_reader_sample_id(BGenFile *bgenfile, uint64_t idx, char **id,
                              uint64_t *length)
{
    if (idx >= bgen_reader_nsamples(bgenfile)) return EXIT_FAILURE;

    SampleId *sampleid = &(bgenfile->sampleid_block.sampleids[idx]);

    *length = sampleid->length;
    *id     = sampleid->id;

    return 0;
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
    *rsid     = vb.rsid;

    fclose(f);
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
    *chrom     = vb.chrom;

    fclose(f);
}
