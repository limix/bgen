#ifndef BGEN_LAYOUT_TWO_H
#define BGEN_LAYOUT_TWO_H

#include "bgen/bgen.h"
#include "index.h"
#include <stdio.h>

int  read_probs_header_two(struct bgen_vi*, struct bgen_vg*, FILE*);
void read_probs_two(struct bgen_vg*, double*);

#endif
