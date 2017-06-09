#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bgen_reader.h"
#include "file.h"
#include "util.h"


// Header
//
// ------------------------------
// | 4       | block length, Lh |
// | 4       | # variants       |
// | 4       | # samples        |
// | 4       | magic number     |
// | Lh - 20 | free data area   |
// | 4       | flags            |
// ------------------------------
int64_t read_header(Header *header, FILE *restrict f, char *filepath)
{
    if (fread_check(&(header->header_length), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(header->nvariants), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(header->nsamples), 4, f, filepath)) return EXIT_FAILURE;

    if (fread_check(&(header->magic_number), 4, f, filepath)) return EXIT_FAILURE;

    fseek(f, (header->header_length) - 20, SEEK_CUR);

    if (fread_check(&(header->flags), 4, f, filepath)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

// Sample identifier block
//
// -------------------------------
// | 4   | block length          |
// | 4   | # samples             |
// | 2   | length of sample 1 id |
// | Ls1 | sample 1 id           |
// | 2   | length of sample 2 id |
// | Ls2 | sample 2 id           |
// | ... |                       |
// | 2   | length of sample N id |
// | LsN | sample N id           |
// -------------------------------
int read_sampleid_block(SampleIdBlock *block,
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
    return EXIT_SUCCESS;
}

// Main function, called before anything.
int64_t bgen_reader_read(BGenFile *bgenfile, char *filepath)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;


    if (bgenfile->file == NULL) {
        fprintf(stderr, "File opening failed: %s\n", filepath);
        return EXIT_FAILURE;
    }

    bgenfile->filepath = ft_strdup(filepath);

    uint32_t offset;

    // First four bytes (offset)
    if (fread_check(&offset, 4, f, filepath)) return EXIT_FAILURE;

    if (read_header(&(bgenfile->header), f, filepath)) return EXIT_FAILURE;

    if (bgenfile->header.header_length > offset)
    {
        fprintf(stderr, "Header length is larger then offset's.\n");
        if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;
        return EXIT_FAILURE;
    }

    SampleIdBlock sampleid_block;

    if (bgen_reader_sampleids(bgenfile))
    {
        if (read_sampleid_block(&(bgenfile->sampleid_block), f, filepath))
        {
            if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;
            return EXIT_FAILURE;
        }
    }


    if (bgenfile->variants_start == EOF)
    {
        perror("Could not find variant blocks");
        if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;
        return EXIT_FAILURE;
    }

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

// What layout is that?
// Possible values are 1 and 2.
int64_t bgen_reader_layout(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & (15 << 2)) >> 2;
}

// Is sample identifier block present?
int64_t bgen_reader_sampleids(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & (1 << 31)) >> 31;
}

// Does it use compression? Which type?
// 0: no compression
// 1: zlib's compress()
// 2: zstandard's ZSTD_compress()
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
