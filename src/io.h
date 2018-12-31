#ifndef _BGEN_IO_H
#define _BGEN_IO_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline void *fclose_nul(FILE *fp)
{
    if (fp)
        fclose(fp);
    return NULL;
}

static inline int fwrite1(const void *restrict buffer, size_t size,
                          FILE *restrict stream)
{
    return fwrite(buffer, size, 1, stream) != 1;
}

static inline int fread1(void *restrict buffer, size_t size, FILE *restrict stream)
{
    return fread(buffer, size, 1, stream) != 1;
}

#define perror_fmt(...)                                                                \
    do {                                                                               \
        fputs("Error: ", stderr);                                                      \
        fprintf(stderr, __VA_ARGS__);                                                  \
        fprintf(stderr, ": ");                                                         \
        perror("");                                                                    \
    } while (0)

#define echo(...)                                                                      \
    do {                                                                               \
        printf(__VA_ARGS__);                                                           \
        puts("");                                                                      \
    } while (0)

#define error(...)                                                                     \
    do {                                                                               \
        fputs("Error: ", stderr);                                                      \
        fprintf(stderr, __VA_ARGS__);                                                  \
        fputs("\n", stderr);                                                           \
    } while (0)

#define DECLARE_TYPE_FREAD(TYPE, SUF) int fread_##SUF(FILE *, TYPE *, size_t);

DECLARE_TYPE_FREAD(int, int);
DECLARE_TYPE_FREAD(unsigned, unsigned);
DECLARE_TYPE_FREAD(uint64_t, ui64);
DECLARE_TYPE_FREAD(uint32_t, ui32);
DECLARE_TYPE_FREAD(uint16_t, ui16);

#define DECLARE_TYPE_FWRITE(TYPE, SUF) int fwrite_##SUF(FILE *, TYPE, size_t);

DECLARE_TYPE_FWRITE(int, int);
DECLARE_TYPE_FWRITE(unsigned, unsigned);
DECLARE_TYPE_FWRITE(uint64_t, ui64);
DECLARE_TYPE_FWRITE(uint32_t, ui32);
DECLARE_TYPE_FWRITE(uint16_t, ui16);

#undef DECLARE_TYPE_FREAD
#undef DECLARE_TYPE_FWRITE

#endif /* _BGEN_IO_H */
