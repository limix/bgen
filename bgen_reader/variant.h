#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>
#include "byte.h"

typedef struct
{
    uint32_t length;
    BYTE    *id;
} Allele;

typedef struct
{
    uint32_t *ui_probs;
} SampleProbs;

typedef struct VariantProbsBlock
{
    uint32_t     nsamples;
    uint16_t     nalleles;
    uint8_t      min_ploidy;
    uint8_t      max_ploidy;
    uint8_t     *missingness;
    uint8_t      phased;
    uint8_t      nbits;
    SampleProbs *sample_probs;
} VariantProbsBlock;

typedef struct VariantBlock
{
    uint32_t           nsamples;
    uint16_t           id_length;
    BYTE              *id;
    uint16_t           rsid_length;
    BYTE              *rsid;
    uint16_t           chrom_length;
    BYTE              *chrom;
    uint32_t           position;
    uint16_t           nalleles;
    Allele            *alleles;
    long               genotype_start;
    VariantProbsBlock *vpb;
} VariantBlock;

#endif /* end of include guard: VARIANT_H */
