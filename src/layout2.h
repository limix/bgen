#ifndef BGEN_LAYOUT2_H
#define BGEN_LAYOUT2_H

#include "bgen/bgen.h"
#include "index.h"

int  bgen_layout2_read_header(struct bgen_vi*, struct bgen_vg*, FILE*);
void bgen_layout2_read_genotype(struct bgen_vg*, double*);

#endif
