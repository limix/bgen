#ifndef BGEN_VARIANT_INDEXING_H
#define BGEN_VARIANT_INDEXING_H

#include <stdio.h>

#include "bgen/number.h"

struct VariantIndexing {
    byte *filepath;
    inti compression;
    inti layout;
    inti nsamples;
    inti nvariants;
    inti *start;
};

#endif /* end of include guard: BGEN_VARIANT_INDEXING_H */
