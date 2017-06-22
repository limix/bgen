#ifndef VARIANT_INDEXING_H
#define VARIANT_INDEXING_H

#include <stdio.h>

#include "types.h"

struct VariantIndexing
{
    byte *filepath;
    FILE *file;
    inti  compression;
    inti  layout;
    inti  nsamples;
    inti  nvariants;
    inti *start;
};

#endif /* end of include guard: VARIANT_INDEXING_H */
