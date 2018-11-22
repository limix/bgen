#ifndef FILE_H
#define FILE_H

#include <stdio.h>

static inline void FCLOSE(FILE *fp) {
    if (fp)
        fclose(fp);
}

static inline int FWRITE(const void *restrict buffer, size_t size,
                         FILE *restrict stream) {
    return fwrite(buffer, size, 1, stream) != 1;
}

static inline int FREAD(void *restrict buffer, size_t size, FILE *restrict stream) {
    return fread(buffer, size, 1, stream) != 1;
}

#define PERROR(...)                                                                     \
    fprintf(stderr, __VA_ARGS__);                                                       \
    perror("");

#endif
