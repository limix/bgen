#ifndef _BGEN_FILE_H
#define _BGEN_FILE_H

#include "io.h"
#include <stdio.h>

struct bgen_file {
    char *filepath;
    FILE *file;
    int nvariants;
    int nsamples;
    int compression;
    int layout;
    int contain_sample;
    int samples_start;
    int variants_start;
};

#define bopen_or_leave(BGEN)                                                           \
    if (!(BGEN->file = fopen(BGEN->filepath, "rb"))) {                                 \
        perror_fmt("Could not open bgen file %s", BGEN->filepath);                     \
        goto err;                                                                      \
    }

#endif /* end of include guard: _BGEN_FILE_H */
