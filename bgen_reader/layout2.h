#ifndef LAYOUT2_H
#define LAYOUT2_H

#include "types.h"

typedef struct BGenFile BGenFile;
typedef struct VariantGenotypeBlock VariantGenotypeBlock;

inti bgen_reader_read_genotype_layout2(BGenFile             *bgenfile,
                                          VariantGenotypeBlock *vpb);

inti bgen_reader_read_genotype_layout2_skip(BGenFile *bgenfile);

#endif /* ifndef LAYOUT2_H */
