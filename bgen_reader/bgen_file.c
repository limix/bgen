#include <stdlib.h>
#include <assert.h>

#include "bgen_file.h"


inti bgen_reader_fopen(BGenFile *bgenfile)
{
    assert(bgenfile->file == NULL);
    bgenfile->file = fopen(bgenfile->filepath, "rb");

    if (bgenfile->file == NULL) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

inti bgen_reader_fclose(BGenFile *bgenfile)
{
    assert(bgenfile->file != NULL);
    fclose(bgenfile->file);
    bgenfile->file = NULL;
    return EXIT_SUCCESS;
}

inti bgen_reader_fread(BGenFile *bgenfile, void *restrict buffer, inti size)
{
    inti err = fread(buffer, size, 1, bgenfile->file);

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
