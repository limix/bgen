#ifndef BGEN_VARIANTS_INDEX_H
#define BGEN_VARIANTS_INDEX_H

#include "file.h"
#include <stdint.h>
#include <stdio.h>

#define BGEN_INDEX_NAME "bgen index "
#define BGEN_INDEX_VERSION "02"
#define BGEN_HEADER_LENGTH 13

struct bgen_vi {
    char *filepath;
    uint32_t compression;
    uint32_t layout;
    uint32_t nsamples;
    uint32_t nvariants;
    uint32_t max_nalleles;
    uint64_t *start;
};

struct bgen_vi *new_variants_index(const struct bgen_file *);

#endif /* end of include guard: BGEN_VARIANTS_INDEX_H */
