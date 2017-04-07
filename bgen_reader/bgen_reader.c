#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bgen_reader.h"

int64_t fread_check(void *restrict buffer, size_t size,
                    FILE *restrict stream, char *filepath)
{
    size_t err = fread(buffer, size, 1, stream);

    if (err != 1)
    {
        if (feof(stream))
        {
            fprintf(stderr,
                    "Error reading %s: unexpected end of file.\n",
                    filepath);
            return -1;
        }
        fprintf(stderr, "Unknown error while reading %s.\n", filepath);
        return -1;
    }
    return 0;
}

int64_t read_header(Header *header, FILE *restrict f, char *filepath)
{
    if (fread_check(&(header->header_length), 4, f, filepath)) return -1;

    if (fread_check(&(header->nvariants), 4, f, filepath)) return -1;

    if (fread_check(&(header->nsamples), 4, f, filepath)) return -1;

    if (fread_check(&(header->magic_number), 4, f, filepath)) return -1;

    fseek(f, (header->header_length) - 20, SEEK_CUR);

    if (fread_check(&(header->flags), 4, f, filepath)) return -1;

    printf("header_length: %u\n", (header->header_length));
    printf("nvariants: %u\n",     (header->nvariants));
    printf("nsamples: %u\n",      (header->nsamples));
    printf("magic_number: %u\n",  (header->magic_number));
    printf("flags: %u\n",         (header->flags));

    return 0;
}

int snp_block_compression(Header *header)
{
    return header->flags & 3;
}

int snp_block_layout(Header *header)
{
    return (header->flags & (4 + 8 + 16 + 32)) >> 2;
}

int has_sample_identifier(Header *header)
{
    return (header->flags & - 1) >> 31;
}

int read_sample_identifier_block(SampleIdBlock *block,
                                 FILE *restrict f,
                                 char          *filepath)
{
    if (fread_check(&(block->length), 4, f, filepath)) return -1;

    if (fread_check(&(block->nsamples), 4, f, filepath)) return -1;

    printf("sampleid_length: %d\n",   block->length);
    printf("sampleid_nsamples: %d\n", block->nsamples);

    block->sampleids = malloc(block->nsamples * sizeof(SampleId));

    assert(sizeof(char) == 1);

    for (size_t i = 0; i < block->nsamples; i++)
    {
        uint16_t *length = &(block->sampleids[i].length);

        if (fread_check(length, 2, f, filepath)) return -1;


        block->sampleids[i].id =
            malloc(block->sampleids[i].length * sizeof(char));

        if (fread_check(block->sampleids[i].id, block->sampleids[i].length, f,
                        filepath)) return -1;
    }
    return 0;
}

int64_t bgen_reader_read(BGenFile *bgenfile)
{
    char *fp = "/Users/horta/workspace/bgen-reader/test/example.1bits.bgen";
    FILE *f  = fopen(fp, "rb");

    uint32_t offset;

    if (fread_check(&offset, 4, f, fp)) return -1;

    printf("offset: %u\n", offset);

    if (read_header(&(bgenfile->header), f, fp)) return -1;


    printf("snp_block_compression: %d\n",
           snp_block_compression(&(bgenfile->header)));
    printf("snp_block_layout: %d\n", snp_block_layout(&(bgenfile->header)));
    printf("has_sample_identifier: %d\n",
           has_sample_identifier(&(bgenfile->header)));

    SampleIdBlock sampleid_block;

    if (has_sample_identifier(&(bgenfile->header)))
        if (read_sample_identifier_block(&(bgenfile->sampleid_block), f,
                                         fp)) return -1;


    fclose(f);
    return 0;
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
    if (idx >= bgen_reader_nsamples(bgenfile)) return -1;

    SampleId *sampleid = &(bgenfile->sampleid_block.sampleids[idx]);

    *length = sampleid->length;
    *id     = sampleid->id;

    return 0;
}
