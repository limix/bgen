#ifndef BGEN_LAYOUT2_H
#define BGEN_LAYOUT2_H

struct bgen_file;
struct bgen_genotype;

int  bgen_layout2_read_header(struct bgen_file* bgen_file, struct bgen_genotype* genotype);
void bgen_layout2_read_genotype(struct bgen_genotype* genotype, double* probabilities);

#endif
