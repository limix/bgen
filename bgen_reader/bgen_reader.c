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

    return 0;
}

int read_sample_identifier_block(SampleIdBlock *block,
                                 FILE *restrict f,
                                 char          *filepath)
{
    if (fread_check(&(block->length), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(block->nsamples), 4, f, filepath)) return EXIT_FAILURE;

    block->sampleids = malloc(block->nsamples * sizeof(SampleId));

    for (size_t i = 0; i < block->nsamples; i++)
    {
        uint16_t *length = &(block->sampleids[i].length);

        if (fread_check(length, 2, f, filepath)) return EXIT_FAILURE;


        block->sampleids[i].id =
            malloc(block->sampleids[i].length);

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

    if (read_header(&(bgenfile->header), f, filepath)) return EXIT_FAILURE;

    SampleIdBlock sampleid_block;

    if (bgen_reader_sample_identifiers(bgenfile))
        if (read_sample_identifier_block(&(bgenfile->sampleid_block), f,
                                         filepath)) return EXIT_FAILURE;

    bgenfile->variants_start = ftell(f);

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
    return (bgenfile->header.flags & (15 << 2)) >> 2;
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
