#ifndef VARIANT_ID_BLOCK_H
#define VARIANT_ID_BLOCK_H

#include "types.h"

typedef struct VariantIdBlock
{
    inti                   nsamples;
    inti                   id_length;
    byte                  *id;
    uint16_t               rsid_length;
    byte                  *rsid;
    uint16_t               chrom_length;
    byte                  *chrom;
    uint32_t               position;
    uint16_t               nalleles;
    uint32_t              *allele_lengths;
    byte                 **alleleids;
    long                   genotype_start;
    struct VariantIdBlock *next;
} VariantIdBlock;

#endif /* end of include guard: VARIANT_ID_BLOCK_H */
