#include "file.h"

#include <assert.h>
#include <stdlib.h>

int64_t bgen_fopen(BGenFile *bgenfile)
{
    assert(bgenfile->file == NULL);
    bgenfile->file = fopen(bgenfile->filepath, "rb");

    if (bgenfile->file == NULL) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int64_t bgen_fclose(BGenFile *bgenfile)
{
    assert(bgenfile->file != NULL);
    fclose(bgenfile->file);
    bgenfile->file = NULL;
    return EXIT_SUCCESS;
}

int64_t fread_check(BGenFile *bgenfile, void *restrict buffer, size_t size)
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
