/*
 * Open, query, and close a variant genotype.
 */
#ifndef BGEN_GENO_H
#define BGEN_GENO_H

#include "api.h"
#include "meta.h"
#include <stddef.h>

struct bgen_vi; /* variant index */
struct bgen_vg; /* variant genotype */

/* Open a variant for genotype queries. */
BGEN_API struct bgen_vg *bgen_open_genotype(struct bgen_file *, long);
/* Close a variant genotype handler. */
BGEN_API void bgen_close_genotype(struct bgen_vg *vg);
/* Read the probabilities of each possible genotype. */
BGEN_API int bgen_read_genotype(struct bgen_file *, struct bgen_vg *, double *);
/* Get the number of alleles. */
BGEN_API int bgen_nalleles(const struct bgen_vg *vg);
/* Return 1 if variant is missing for the sample; 0 otherwise. */
BGEN_API int bgen_missing(const struct bgen_vg *vg, size_t index);
/* Get the ploidy. */
BGEN_API int bgen_ploidy(const struct bgen_vg *vg, size_t index);
/* Get the minimum ploidy of the variant. */
BGEN_API int bgen_min_ploidy(const struct bgen_vg *vg);
/* Get the maximum ploidy of the variant. */
BGEN_API int bgen_max_ploidy(const struct bgen_vg *vg);
/* Get the number of genotype combinations. */
BGEN_API int bgen_ncombs(const struct bgen_vg *vg);
/* Return 1 for phased or 0 for unphased genotype. */
BGEN_API int bgen_phased(const struct bgen_vg *vg);

#endif /* BGEN_GENO_H */
