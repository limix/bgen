#include "file.h"

int64_t bgen_fopen(BGenFile *bgenfile)
{
    assert(bgenfile->file == NULL);
    bgenfile->file = fopen(bgenfile->filepath, "rb");

    if (bgenfile->file == NULL) {
        fprintf(stderr, "File opening failed: %s\n", filepath);
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
            return EXIT_FAILURE;
        }
        fprintf(stderr, "Unknown error while reading %s.\n", filepath);
        return EXIT_FAILURE;
    }
    return 0;
}
