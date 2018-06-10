#ifndef BGEN_VARIANT_GENOTYPE_H
#define BGEN_VARIANT_GENOTYPE_H

#include "util/stdint.h"
#include <stddef.h>

struct bgen_vg {
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t phased;
    uint8_t nbits;
    uint8_t *plo_miss;
    int ncombs;
    uint8_t min_ploidy;
    uint8_t max_ploidy;
    char *chunk;
    char *current_chunk;
    size_t variant_idx;
} bgen_vg;

#endif /* end of include guard: BGEN_VARIANT_GENOTYPE_H */
