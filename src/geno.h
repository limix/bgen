#ifndef _BGEN_VARIANT_GENOTYPE_H
#define _BGEN_VARIANT_GENOTYPE_H

#include <stddef.h>
#include <stdint.h>

struct bgen_vg
{
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
    /* Deprecated: used by bgen versions 1 and 2. Use `vaddr` instead. */
    size_t variant_idx;
    size_t vaddr;
};

#endif /* _BGEN_VARIANT_GENOTYPE_H */
