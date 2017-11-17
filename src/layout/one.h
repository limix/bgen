#ifndef BGEN_LAYOUT_ONE_H
#define BGEN_LAYOUT_ONE_H

#include <stdio.h>

#include "bgen/number.h"

typedef struct VariantGenotype VariantGenotype;
typedef struct VariantIndexing VariantIndexing;

inti bgen_read_variant_genotype_header_layout1(VariantIndexing *indexing,
                                               VariantGenotype *vg, FILE *file);

void bgen_read_variant_genotype_probabilities_layout1(VariantIndexing *indexing,
                                                      VariantGenotype *vg,
                                                      real *probabilities);

#endif /* ifndef BGEN_LAYOUT_ONE_H */
