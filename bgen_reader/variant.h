#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>

#include "variant_id_block.h"
#include "types.h"

typedef struct VariantGenotypeBlock
{
    inti  nsamples;
    inti  nalleles;
    inti  min_ploidy;
    inti  max_ploidy;
    inti *missingness;
    inti  phased;
    inti  nbits;
    inti *genotypes;
} VariantGenotypeBlock;

// It represents both the variant identifying block
// (all fields except the last one) and the variant probabilities block (the
// last field)
typedef struct VariantBlock
{
    VariantIdBlock        id_block;
    VariantGenotypeBlock *vpb;
} VariantBlock;

typedef struct BGenFile BGenFile;

inti bgen_reader_read_variantid_block(BGenFile       *bgenfile,
                                      inti            variant_idx,
                                      VariantIdBlock *vib);

inti bgen_reader_seek_variant_block(BGenFile *bgenfile,
                                    inti      variant_idx);

inti bgen_reader_read_current_variantid_block(BGenFile       *bgenfile,
                                              VariantIdBlock *vib);

inti bgen_reader_read_current_genotype_block(BGenFile *bgenfile,
                                             inti     *ploidy,
                                             inti     *nalleles,
                                             inti     *nbits,
                                             inti    **ui_probs);

inti bgen_reader_genotype_skip(BGenFile *bgenfile);

#endif /* end of include guard: VARIANT_H */
