#ifndef BGEN_LAYOUT_TWO_H
#define BGEN_LAYOUT_TWO_H

#include <stdio.h>

#include "bgen.h"

int bgen_read_probs_header_two(struct bgen_vi *, struct bgen_vg *, FILE *);
void bgen_read_probs_two(struct bgen_vg *, double *);

#endif /* ifndef BGEN_LAYOUT_TWO_H */
