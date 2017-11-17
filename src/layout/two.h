#ifndef BGEN_LAYOUT_TWO_H
#define BGEN_LAYOUT_TWO_H

#include <stdio.h>

#include "bgen/number.h"

typedef struct VariantGenotype VariantGenotype;
typedef struct VariantIndexing VariantIndexing;

inti bgen_read_variant_genotype_header_layout2(VariantIndexing *indexing,
                                               VariantGenotype *vg, FILE *file);

void bgen_read_variant_genotype_probabilities_layout2(VariantIndexing *indexing,
                                                      VariantGenotype *vg,
                                                      real *probabilities);

#endif /* ifndef BGEN_LAYOUT_TWO_H */
