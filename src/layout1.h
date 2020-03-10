#ifndef BGEN_LAYOUT1_H
#define BGEN_LAYOUT1_H

#include "bgen/bgen.h"
#include "index.h"
#include <stdio.h>

int  bgen_layout1_read_header(struct bgen_vi*, struct bgen_genotype*, FILE*);
void bgen_layout1_read_genotype(struct bgen_genotype*, double*);

#endif
