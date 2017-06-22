#ifndef VARIANT_GENOTYPE_H
#define VARIANT_GENOTYPE_H

#include "variant_genotype_fwd.h"

struct VariantGenotype
{
    inti     nsamples;
    inti     nalleles;
    inti     nbits;
    uint8_t *plo_miss;
    inti     ncombs;
    inti     ploidy;
    byte    *chunk;
    byte    *current_chunk;
    inti     variant_idx;
};

#endif /* end of include guard: VARIANT_GENOTYPE_H */
