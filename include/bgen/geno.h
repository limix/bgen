/** Open, query, and close a variant genotype.
 * @file bgen/geno.h
 */
#ifndef BGEN_GENO_H
#define BGEN_GENO_H

#include "api.h"
#include "meta.h"
#include <stddef.h>

/** Variant index.
 * @struct bgen_vi
 */
struct bgen_vi;
/** Variant genotype.
 * @struct bgen_vg
 */
struct bgen_vg;

/** Open a variant for genotype queries.
 *
 * @param bgen Bgen file handler.
 * @param vaddr Variant address in the bgen file.
 * @return Variant genotype handler. Return `NULL` on failure.
 */
BGEN_API struct bgen_vg *bgen_open_genotype(struct bgen_file *bgen, long vaddr);
/** Close a variant genotype handler.
 *
 * @param vg Variant genotype handler.
 */
BGEN_API void bgen_close_genotype(struct bgen_vg *vg);
/** Read the probabilities of each possible genotype.
 *
 * The length of this array is equal to the product of the values obtained by calling
 * the functions @ref bgen_nsamples and @ref bgen_ncombs.
 * \rst
 * .. seealso::
 *     Please, refer to the corresponding section **Probability data storage** of the
 *     |bgen format specification| for more information.
 * \endrst
 *
 * @param bgen Bgen file handler.
 * @param vg Variant genotype handler.
 * @param p Array of probabilities.
 * @return `0` if it succeeded; `1` otherwise.
 */
BGEN_API int bgen_read_genotype(struct bgen_file *bgen, struct bgen_vg *vg, double *p);
/** Get the number of alleles.
 *
 * @param vg Variant genotype handler.
 * @return Number of alleles.
 */
BGEN_API int bgen_nalleles(const struct bgen_vg *vg);
/** Return `1` if variant is missing for the sample; `0` otherwise.
 *
 * @param vg Variant genotype handler.
 * @param index Sample index.
 * @return `1` for missing genotype; `0` otherwise.
 */
BGEN_API int bgen_missing(const struct bgen_vg *vg, int index);
/** Get the ploidy.
 *
 * @param vg Variant genotype handler.
 * @param index Sample index.
 * @return Ploidy.
 */
BGEN_API int bgen_ploidy(const struct bgen_vg *vg, int index);
/** Get the minimum ploidy of the variant.
 *
 * @param vg Variant genotype handler.
 * @return Ploidy minimum.
 */
BGEN_API int bgen_min_ploidy(const struct bgen_vg *vg);
/** Get the maximum ploidy of the variant.
 *
 * @param vg Variant genotype handler.
 * @return Ploidy maximum.
 */
BGEN_API int bgen_max_ploidy(const struct bgen_vg *vg);
/** Get the number of genotype combinations.
 *
 * Precisely, if the bgen file is of **Layout 1**, the number of combinations is always
 * equal to `3`. In the case of **Layout 2**, we have two options. For phased genotype,
 * the number of combinations is equal to the product of @ref bgen_nalleles with
 * @ref bgen_max_ploidy. For unphased genotype, let `n` and `m` be the values returned
 * by calling @ref bgen_nalleles and @ref bgen_max_ploidy. This function returns the
 * number of combinations `n-1` alleles can be selected from `n+m-1`, such that the
 * order of a selection does not matter.
 *
 * @param vg Variant genotype handler.
 * @return Number of combinations.
 */
BGEN_API int bgen_ncombs(const struct bgen_vg *vg);
/** Return `1` for phased or `0` for unphased genotype.
 *
 * @param vg Variant genotype handler.
 * @return `1` for phased genotype; `0` otherwise.
 */
BGEN_API int bgen_phased(const struct bgen_vg *vg);

#endif /* BGEN_GENO_H */
