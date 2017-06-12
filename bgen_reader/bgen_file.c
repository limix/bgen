#include <stdlib.h>
#include <assert.h>

#include "bgen_file.h"

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
int64_t bgen_reader_read_header(BGenFile *bgenfile, Header *header)
{
    if (bgen_reader_fread(bgenfile, &(header->header_length), 4)) return EXIT_FAILURE;

    if (bgen_reader_fread(bgenfile, &(header->nvariants), 4)) return EXIT_FAILURE;

    if (bgen_reader_fread(bgenfile, &(header->nsamples), 4)) return EXIT_FAILURE;

    if (bgen_reader_fread(bgenfile, &(header->magic_number), 4)) return EXIT_FAILURE;

    fseek(bgenfile->file, (header->header_length) - 20, SEEK_CUR);

    if (bgen_reader_fread(bgenfile, &(header->flags), 4)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_fopen(BGenFile *bgenfile)
{
    assert(bgenfile->file == NULL);
    bgenfile->file = fopen(bgenfile->filepath, "rb");

    if (bgenfile->file == NULL) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int64_t bgen_reader_fclose(BGenFile *bgenfile)
{
    assert(bgenfile->file != NULL);
    fclose(bgenfile->file);
    bgenfile->file = NULL;
    return EXIT_SUCCESS;
}

int64_t bgen_reader_fread(BGenFile *bgenfile, void *restrict buffer, size_t size)
{
    size_t err = fread(buffer, size, 1, bgenfile->file);

    if (err != 1)
    {
        if (feof(bgenfile->file))
        {
            fprintf(stderr,
                    "Error reading %s: unexpected end of file.\n",
                    bgenfile->filepath);
            return EXIT_FAILURE;
        }
        fprintf(stderr, "Unknown error while reading %s.\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
