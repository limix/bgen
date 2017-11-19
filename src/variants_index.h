#ifndef BGEN_VARIANTS_INDEX_H
#define BGEN_VARIANTS_INDEX_H

#include <stdio.h>

#include "bgen/number.h"

struct BGenVI {
    byte *filepath;
    inti compression;
    inti layout;
    inti nsamples;
    inti nvariants;
    inti *start;
};

struct BGenVI *bgen_new_index(const struct BGenFile *);

#endif /* end of include guard: BGEN_VARIANTS_INDEX_H */
