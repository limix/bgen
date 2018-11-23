#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>

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

#define echo(...)                                                                       \
    do {                                                                                \
        printf(__VA_ARGS__);                                                            \
        puts("");                                                                       \
    } while (0)

#define error(...)                                                                      \
    do {                                                                                \
        fprintf(stderr, __VA_ARGS__);                                                   \
        fputs("", stderr);                                                              \
    } while (0)

#define DECLARE_TYPE_FREAD(TYPE) int TYPE##_fread(FILE *, TYPE *, size_t);

DECLARE_TYPE_FREAD(int);
DECLARE_TYPE_FREAD(unsigned);
DECLARE_TYPE_FREAD(uint64_t);
DECLARE_TYPE_FREAD(uint32_t);
DECLARE_TYPE_FREAD(uint16_t);

#define DECLARE_TYPE_FWRITE(TYPE) int TYPE##_fwrite(FILE *, TYPE, size_t);

DECLARE_TYPE_FWRITE(int);
DECLARE_TYPE_FWRITE(unsigned);
DECLARE_TYPE_FWRITE(uint64_t);
DECLARE_TYPE_FWRITE(uint32_t);
DECLARE_TYPE_FWRITE(uint16_t);

#undef DECLARE_TYPE_FREAD
#undef DECLARE_TYPE_FWRITE

#endif
