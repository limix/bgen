#ifndef BGEN_GENOTYPE_H_PRIVATE
#define BGEN_GENOTYPE_H_PRIVATE

#include <stdint.h>
#include <stdlib.h>

struct bgen_genotype
{
    unsigned    layout;
    uint32_t    nsamples;
    uint16_t    nalleles;
    uint8_t     phased;
    uint8_t     nbits;
    uint8_t*    ploidy_missingness;
    unsigned    ncombs;
    uint8_t     min_ploidy;
    uint8_t     max_ploidy;
    char*       chunk;
    char const* chunk_ptr;
    uint64_t    offset;
};

static inline struct bgen_genotype* bgen_genotype_create(void)
{
    struct bgen_genotype* genotype = malloc(sizeof(struct bgen_genotype));
    genotype->layout = 0;
    genotype->nsamples = 0;
    genotype->nalleles = 0;
    genotype->phased = 0;
    genotype->nbits = 0;
    genotype->ploidy_missingness = NULL;
    genotype->ncombs = 0;
    genotype->min_ploidy = 0;
    genotype->max_ploidy = 0;
    genotype->chunk = NULL;
    genotype->chunk_ptr = NULL;
    genotype->offset = 0;
    return genotype;
}

#endif
