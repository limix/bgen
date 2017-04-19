#include "file.h"

#include <stdlib.h>

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
