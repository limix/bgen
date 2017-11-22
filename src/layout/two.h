#ifndef BGEN_LAYOUT_TWO_H
#define BGEN_LAYOUT_TWO_H

#include <stdio.h>

#include "bgen/bgen.h"

int bgen_read_probs_header_two(struct BGenVI *, struct BGenVG *, FILE *);
void bgen_read_probs_two(struct BGenVG *, double *);

#endif /* ifndef BGEN_LAYOUT_TWO_H */
