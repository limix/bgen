#ifndef _BGEN_LAYOUT_ONE_H
#define _BGEN_LAYOUT_ONE_H

#include "bgen.h"
#include <stdio.h>

int bgen_read_probs_header_one(struct bgen_vi *, struct bgen_vg *, FILE *);
void bgen_read_probs_one(struct bgen_vg *, double *);

#endif /* _BGEN_LAYOUT_ONE_H */
