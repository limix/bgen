#ifndef BGEN_LAYOUT1_H
#define BGEN_LAYOUT1_H

struct bgen_file;
struct bgen_genotype;

int  bgen_layout1_read_header(struct bgen_file* bgen_file, struct bgen_genotype* genotype);
void bgen_layout1_read_genotype64(struct bgen_genotype* genotype, double* probs);
void bgen_layout1_read_genotype32(struct bgen_genotype* genotype, float* probs);

#endif
