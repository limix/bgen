#ifndef BGEN_LAYOUT_ONE_H
#define BGEN_LAYOUT_ONE_H

#include <stdio.h>

#include "bgen/bgen.h"

int bgen_read_probs_header_one(struct BGenVI *, struct BGenVG *, FILE *);
void bgen_read_probs_one(struct BGenVG *, double *);

#endif /* ifndef BGEN_LAYOUT_ONE_H */
