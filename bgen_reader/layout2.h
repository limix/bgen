#ifndef LAYOUT2_H
#define LAYOUT2_H

#include <stdint.h>

typedef struct BGenFile BGenFile;
typedef struct VariantGenotypeBlock VariantGenotypeBlock;

int64_t bgen_reader_read_genotype_layout2(BGenFile             *bgenfile,
                                          VariantGenotypeBlock *vpb);

int64_t bgen_reader_read_genotype_layout2_skip(BGenFile *bgenfile);

#endif /* ifndef LAYOUT2_H */
