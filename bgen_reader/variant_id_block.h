#ifndef VARIANT_ID_BLOCK_H
#define VARIANT_ID_BLOCK_H

#include "types.h"

typedef struct VariantIdBlock
{
    inti                   nsamples;
    inti                   id_length;
    byte                  *id;
    inti                   rsid_length;
    byte                  *rsid;
    inti                   chrom_length;
    byte                  *chrom;
    inti                   position;
    inti                   nalleles;
    inti                  *allele_lengths;
    byte                 **alleleids;
    inti                   genotype_start;
    struct VariantIdBlock *next;
} VariantIdBlock;

#endif /* end of include guard: VARIANT_ID_BLOCK_H */
