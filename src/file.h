#ifndef _BGEN_FILE_H
#define _BGEN_FILE_H

#include "bgen/bgen.h"
#include "io.h"
#include <assert.h>
#include <stdio.h>

FILE* bgen_file_stream(struct bgen_file const* bgen_file);
char const* bgen_file_filepath(struct bgen_file const* bgen_file);
int bgen_file_layout(struct bgen_file const* bgen_file);
int bgen_file_compression(struct bgen_file const* bgen_file);
int bgen_file_seek_variants_start(struct bgen_file* bgen_file);

#define bopen_or_leave(BGEN)                                                                  \
    if (!(BGEN->file = fopen(BGEN->filepath, "rb"))) {                                        \
        perror_fmt(BGEN->file, "Could not open bgen file %s", BGEN->filepath);                \
        goto err;                                                                             \
    }

int close_bgen_file(struct bgen_file* bgen);

#endif /* _BGEN_FILE_H */
