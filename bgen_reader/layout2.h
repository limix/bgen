#ifndef LAYOUT2_H
#define LAYOUT2_H

#include "variant.h"

int64_t bgen_genotype_block_layout2(BGenFile          *bgenfile,
                                    int64_t            compression,
                                    int64_t            nsamples,
                                    VariantGenotypeBlock *vpb,
                                    uint32_t          **ui_probs);

int64_t bgen_genotype_block_layout2_skip(BGenFile *bgenfile,
                                         int64_t   compression,
                                         int64_t   nsamples);

#endif /* ifndef LAYOUT2_H */
