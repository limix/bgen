#ifndef _BGEN_FILE_H
#define _BGEN_FILE_H

#include "bgen/bgen.h"
#include "io.h"
#include <assert.h>
#include <stdio.h>

struct bgen_file
{
    char *filepath;
    FILE *file;
    int nvariants;
    int nsamples;
    int compression;
    int layout;
    int contain_sample;
    OFF_T samples_start;
    OFF_T variants_start;
};

#define bopen_or_leave(BGEN)                                                           \
    if (!(BGEN->file = fopen(BGEN->filepath, "rb"))) {                                 \
        perror_fmt(BGEN->file, "Could not open bgen file %s", BGEN->filepath);         \
        goto err;                                                                      \
    }

int close_bgen_file(struct bgen_file *bgen);

#endif /* _BGEN_FILE_H */
