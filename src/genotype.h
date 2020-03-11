#ifndef BGEN_GENO_H_PRIVATE
#define BGEN_GENO_H_PRIVATE

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

static inline struct bgen_genotype* bgen_genotype_create(void)
{
    struct bgen_genotype* vg = malloc(sizeof(struct bgen_genotype));
    vg->layout = 0;
    vg->nsamples = 0;
    vg->nalleles = 0;
    vg->phased = 0;
    vg->nbits = 0;
    vg->plo_miss = NULL;
    vg->ncombs = 0;
    vg->min_ploidy = 0;
    vg->max_ploidy = 0;
    vg->chunk = NULL;
    vg->current_chunk = NULL;
    vg->offset = 0;
    return vg;
}

#endif
