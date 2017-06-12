#ifndef LAYOUT2_H
#define LAYOUT2_H

#include "variant.h"

int64_t bgen_reader_read_genotype_layout2(BGenFile          *bgenfile,
                                    int64_t            compression,
                                    VariantGenotypeBlock *vpb,
                                    uint32_t          **ui_probs);

int64_t bgen_reader_read_genotype_layout2_skip(BGenFile *bgenfile,
                                         int64_t   compression);

#endif /* ifndef LAYOUT2_H */
