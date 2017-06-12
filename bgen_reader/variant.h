#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>

#include "util/byte.h"

typedef struct
{
    uint32_t length;
    BYTE    *id;
} Allele;

typedef struct VariantGenotypeBlock
{
    uint32_t  nsamples;
    uint16_t  nalleles;
    uint8_t   min_ploidy;
    uint8_t   max_ploidy;
    uint8_t  *missingness;
    uint8_t   phased;
    uint8_t   nbits;
    uint32_t *genotypes;
} VariantGenotypeBlock;

// It represents both the variant identifying block
// (all fields except the last one) and the variant probabilities block (the
// last field)
typedef struct VariantBlock
{
    uint32_t              nsamples;
    uint16_t              id_length;
    BYTE                 *id;
    uint16_t              rsid_length;
    BYTE                 *rsid;
    uint16_t              chrom_length;
    BYTE                 *chrom;
    uint32_t              position;
    uint16_t              nalleles;
    Allele               *alleles;
    long                  genotype_start;
    VariantGenotypeBlock *vpb;
} VariantBlock;

typedef struct BGenFile BGenFile;

int64_t bgen_reader_read_variantid_block(BGenFile     *bgenfile,
                                         uint64_t      variant_idx,
                                         VariantBlock *vb);

int64_t bgen_reader_read_current_genotype_block(BGenFile  *bgenfile,
                                                uint32_t **ui_probs);

#endif /* end of include guard: VARIANT_H */
