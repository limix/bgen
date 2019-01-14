#ifndef _BGEN_FILE_H
#define _BGEN_FILE_H

#include "bgen.h"
#include "io.h"
#include "platform.h"
#include <assert.h>
#include <stdio.h>

// Avoid fseek()'s 2GiB barrier with MSVC, macOS, *BSD, MinGW
// Credits to ZSTD library
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define LONG_SEEK _fseeki64
#elif !defined(__64BIT__) &&                                                           \
    (PLATFORM_POSIX_VERSION >= 200112L) /* No point defining Large file for 64 bit */
#define LONG_SEEK fseeko
#elif defined(__MINGW32__) && !defined(__STRICT_ANSI__) && !defined(__NO_MINGW_LFS) && \
    defined(__MSVCRT__)
#define LONG_SEEK fseeko64
#elif defined(_WIN32) && !defined(__DJGPP__)
static_assert(0, "Code does not work on the DJGPP compiler.");
#else
#define LONG_SEEK fseek
#endif

struct bgen_file
{
    char *filepath;
    FILE *file;
    int nvariants;
    int nsamples;
    int compression;
    int layout;
    int contain_sample;
    int sample_ids_presence BGEN_DEPRECATED;
    int samples_start;
    int variants_start;
};

#define bopen_or_leave(BGEN)                                                           \
    if (!(BGEN->file = fopen(BGEN->filepath, "rb"))) {                                 \
        perror_fmt("Could not open bgen file %s", BGEN->filepath);                     \
        goto err;                                                                      \
    }

int close_bgen_file(struct bgen_file *bgen);

#endif /* _BGEN_FILE_H */
