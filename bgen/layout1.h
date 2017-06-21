#ifndef LAYOUT1_H
#define LAYOUT1_H

#include "types.h"

typedef struct VariantIndexing VariantIndexing;
typedef struct VariantGenotype VariantGenotype;

inti bgen_read_variant_genotype_header_layout1(VariantIndexing *indexing,
                                               VariantGenotype *vg);

void bgen_read_variant_genotype_probabilities_layout1(VariantIndexing *indexing,
                                                      VariantGenotype *vg,
                                                      real            *probabilities);

#endif /* ifndef LAYOUT1_H */
