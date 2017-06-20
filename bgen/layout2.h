#ifndef LAYOUT2_H
#define LAYOUT2_H

#include "types.h"

typedef struct VariantIndexing VariantIndexing;
typedef struct VariantGenotype VariantGenotype;

inti bgen_read_variant_genotype_header(VariantIndexing *indexing,
                                       VariantGenotype *vg);

void bgen_read_variant_genotype_probabilities(VariantIndexing *indexing,
                                              VariantGenotype *vg,
                                              real            *probabilities);

#endif /* ifndef LAYOUT2_H */
