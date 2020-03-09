/* Define low-level io read and write functions. */
#ifndef _BGEN_IO_H
#define _BGEN_IO_H

#include <stdint.h>
#include <stdio.h>

// Avoid fseek()'s 2GiB barrier with MSVC, macOS, *BSD, MinGW
// Credits to ZSTD library
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define LONG_SEEK _fseeki64
#define LONG_TELL _ftelli64
typedef __int64 OFF_T;
#elif !defined(__64BIT__) &&                                                           \
    (PLATFORM_POSIX_VERSION >= 200112L) /* No point defining Large file for 64 bit */
#define LONG_SEEK fseeko
#define LONG_TELL ftello
typedef off_t OFF_T;
#elif defined(__MINGW32__) && !defined(__STRICT_ANSI__) && !defined(__NO_MINGW_LFS) && \
    defined(__MSVCRT__)
#define LONG_SEEK fseeko64
#define LONG_TELL ftello64
typedef off64_t OFF_T;
#elif defined(_WIN32) && !defined(__DJGPP__)
static_assert(0, "Code does not work on the DJGPP compiler.");
#else
#define LONG_SEEK fseek
#define LONG_TELL ftell
typedef long OFF_T;
#endif

static inline int fclose_nul(FILE *fp)
{
    if (fp)
        return fclose(fp);
    return 0;
}

/* Write a single block of data with a given size. */
static inline int fwrite1(const void *restrict buffer, size_t size,
                          FILE *restrict stream)
{
    return fwrite(buffer, size, 1, stream) != 1;
}

/* Read a single block of data with a given size. */
static inline int fread1(void *restrict buffer, size_t size, FILE *restrict stream)
{
    return fread(buffer, size, 1, stream) != 1;
}

#define perror_fmt(fp, ...)                                                            \
    do {                                                                               \
        fputs("Error: ", stderr);                                                      \
        fprintf(stderr, __VA_ARGS__);                                                  \
        fprintf(stderr, ": ");                                                         \
        if (!fp)                                                                       \
            fprintf(stderr, "NULL pointer\n");                                         \
        else if (feof(fp))                                                             \
            fprintf(stderr, "unexpected end of file\n");                               \
        else if (ferror(fp))                                                           \
            perror("");                                                                \
        else                                                                           \
            fprintf(stderr, "unknown\n");                                              \
    } while (0)

#define echo(...)                                                                      \
    do {                                                                               \
        printf(__VA_ARGS__);                                                           \
        puts(".");                                                                     \
    } while (0)

#define warn(...)                                                                      \
    do {                                                                               \
        fputs("Warning: ", stdout);                                                    \
        printf(__VA_ARGS__);                                                           \
        puts(".");                                                                     \
    } while (0)

#define error(...)                                                                     \
    do {                                                                               \
        fputs("Error: ", stderr);                                                      \
        fprintf(stderr, __VA_ARGS__);                                                  \
        fputs(".\n", stderr);                                                          \
    } while (0)

/* Define fread_int, fread_ui64 and like. */
#define DECLARE_TYPE_FREAD(TYPE, SUF) int fread_##SUF(FILE *, TYPE *, size_t);
#define DECLARE_TYPE_FREAD_SIZE(TYPE, SUF) int fread_##SUF(FILE *, TYPE *);

DECLARE_TYPE_FREAD(int, int)
DECLARE_TYPE_FREAD(long, long)
DECLARE_TYPE_FREAD(unsigned, unsigned)
DECLARE_TYPE_FREAD(OFF_T, off)
DECLARE_TYPE_FREAD(uint64_t, ui64)
DECLARE_TYPE_FREAD(uint32_t, ui32)
DECLARE_TYPE_FREAD(uint16_t, ui16)

/* Define fwrite_int, fwrite_ui64 and like. */
#define DECLARE_TYPE_FWRITE(TYPE, SUF) int fwrite_##SUF(FILE *, TYPE, size_t);

DECLARE_TYPE_FWRITE(int, int)
DECLARE_TYPE_FWRITE(long, long)
DECLARE_TYPE_FWRITE(unsigned, unsigned)
DECLARE_TYPE_FWRITE(OFF_T, off)
DECLARE_TYPE_FWRITE(uint64_t, ui64)
DECLARE_TYPE_FWRITE(uint32_t, ui32)
DECLARE_TYPE_FWRITE(uint16_t, ui16)

#undef DECLARE_TYPE_FREAD
#undef DECLARE_TYPE_FWRITE

#endif /* _BGEN_IO_H */
