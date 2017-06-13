#ifndef VARIANT_ID_BLOCK_H
#define VARIANT_ID_BLOCK_H

#include <stdint.h>

typedef struct VariantIdBlock
{
    uint32_t               nsamples;
    uint16_t               id_length;
    unsigned char         *id;
    uint16_t               rsid_length;
    unsigned char         *rsid;
    uint16_t               chrom_length;
    unsigned char         *chrom;
    uint32_t               position;
    uint16_t               nalleles;
    uint32_t              *allele_lengths;
    unsigned char        **alleleids;
    long                   genotype_start;
    struct VariantIdBlock *next;
} VariantIdBlock;

#endif /* end of include guard: VARIANT_ID_BLOCK_H */
