#ifndef BGEN_GENO_H_PRIVATE
#define BGEN_GENO_H_PRIVATE

#include "io.h"
#include <stdint.h>
#include <stdlib.h>

struct bgen_genotype
{
    unsigned layout;
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t  phased;
    uint8_t  nbits;
    uint8_t* plo_miss;
    unsigned ncombs;
    uint8_t  min_ploidy;
    uint8_t  max_ploidy;
    char*    chunk;
    char*    current_chunk;
    uint64_t offset;
};

static inline struct bgen_genotype* create_vg(void)
{
    struct bgen_genotype* vg = malloc(sizeof(struct bgen_genotype));
    vg->plo_miss = NULL;
    vg->chunk = NULL;
    vg->current_chunk = NULL;
    return vg;
}

#endif
