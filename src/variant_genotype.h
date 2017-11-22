#ifndef BGEN_VARIANT_GENOTYPE_H
#define BGEN_VARIANT_GENOTYPE_H

#include <stddef.h>
#include "util/stdint.h"

struct BGenVG {
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t nbits;
    uint8_t *plo_miss;
    int ncombs;
    uint8_t ploidy;
    char *chunk;
    char *current_chunk;
    size_t variant_idx;
} BGenVG;

#endif /* end of include guard: BGEN_VARIANT_GENOTYPE_H */
