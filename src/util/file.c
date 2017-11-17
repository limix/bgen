#include "util/file.h"

inti bgen_read(FILE *file, void *buffer, inti size) {
    inti e = fread(buffer, size, 1, file);

    if (e != 1) {
        if (feof(file)) {
            perror("Unexpected end of file");
            return EXIT_FAILURE;
        }
        fprintf(stderr, "Unknown error while reading the file.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
