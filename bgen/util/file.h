#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#include "../types.h"

static inline inti bgen_read(FILE          *file,
                             void *restrict buffer,
                             inti           size)
{
    inti e = fread(buffer, size, 1, file);

    if (e != 1)
    {
        if (feof(file))
        {
            perror("Unexpected end of file");
            return EXIT_FAILURE;
        }
        fprintf(stderr, "Unknown error while reading the file.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#endif /* end of include guard: FILE_H */
