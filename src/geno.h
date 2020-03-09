#ifndef BGEN_GENO_H_PRIVATE
#define BGEN_GENO_H_PRIVATE

#include "io.h"
#include <stdint.h>

struct bgen_vg
{
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
    OFF_T    vaddr;
};

#endif
