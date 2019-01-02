/*
 * DEPRECATED functions to open, query, and close a variant genotype.
 */
#ifndef BGEN_DEPR_BGEN_H
#define BGEN_DEPR_BGEN_H

#include "api.h"
#include <stddef.h>

struct bgen_vi; /* variant index */
struct bgen_vg; /* variant genotype */

/* Open a variant for genotype queries. */
BGEN_API struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *vi, size_t index);
/* Close a variant genotype handler. */
BGEN_API void bgen_close_variant_genotype(struct bgen_vg *vg);
/* Read the probabilities of each possible genotype. */
BGEN_API int bgen_read_variant_genotype(struct bgen_vi *, struct bgen_vg *, double *);

#endif /* BGEN_DEPR_BGEN_H */
