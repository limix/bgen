#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>

#include "variant_id_block.h"
#include "types.h"

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
    VariantIdBlock       id_block;
    VariantGenotypeBlock *vpb;
} VariantBlock;

typedef struct BGenFile BGenFile;

int64_t bgen_reader_read_variantid_block(BGenFile     *bgenfile,
                                         uint64_t      variant_idx,
                                         VariantIdBlock *vib);

int64_t bgen_reader_seek_variant_block(BGenFile *bgenfile, uint64_t variant_idx);

int64_t bgen_reader_read_current_variantid_block(BGenFile     *bgenfile,
                                                 VariantIdBlock *vib);

int64_t bgen_reader_read_current_genotype_block(BGenFile  *bgenfile,
                                                uint64_t  *ploidy,
                                                uint64_t  *nalleles,
                                                uint64_t  *nbits,
                                                uint32_t **ui_probs);

#endif /* end of include guard: VARIANT_H */
