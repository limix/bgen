#ifndef BGEN_VARIANT_GENOTYPE_H
#define BGEN_VARIANT_GENOTYPE_H

#include "bgen/number.h"

struct BGenVG {
    inti nsamples;
    inti nalleles;
    inti nbits;
    uint8_t *plo_miss;
    inti ncombs;
    inti ploidy;
    byte *chunk;
    byte *current_chunk;
    inti variant_idx;
} BGenVG;

#endif /* end of include guard: BGEN_VARIANT_GENOTYPE_H */
