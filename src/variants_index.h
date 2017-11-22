#ifndef BGEN_VARIANTS_INDEX_H
#define BGEN_VARIANTS_INDEX_H

#include <stdio.h>

struct BGenVI {
    char *filepath;
    uint32_t compression;
    uint32_t layout;
    uint32_t nsamples;
    uint32_t nvariants;
    uint64_t *start;
};

struct BGenVI *new_variants_index(const struct BGenFile *);

#endif /* end of include guard: BGEN_VARIANTS_INDEX_H */
