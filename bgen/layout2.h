#ifndef LAYOUT2_H
#define LAYOUT2_H

#include "types.h"

typedef struct VariantIndexing VariantIndexing;
typedef struct VariantGenotype VariantGenotype;

// inti bgen_reader_layout2_genotype_skip(BGenFile *bgenfile);
inti bgen_read_layout2_genotype(VariantIndexing *indexing,
                                VariantGenotype *vg);

#endif /* ifndef LAYOUT2_H */
