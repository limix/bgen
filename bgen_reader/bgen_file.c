#include <stdlib.h>

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
