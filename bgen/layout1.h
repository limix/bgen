#ifndef LAYOUT1_H
#define LAYOUT1_H

#include "types.h"
#include "variant_genotype_fwd.h"
#include "variant_indexing_fwd.h"

inti bgen_read_variant_genotype_header_layout1(VariantIndexing *indexing,
                                               VariantGenotype *vg);

void bgen_read_variant_genotype_probabilities_layout1(VariantIndexing *indexing,
                                                      VariantGenotype *vg,
                                                      real            *probabilities);

#endif /* ifndef LAYOUT1_H */
