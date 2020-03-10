/** Open, query, and close a variant genotype.
 * @file bgen/geno.h
 */
#ifndef BGEN_GENO_H
#define BGEN_GENO_H

#include "bgen/meta.h"
#include "bgen_export.h"
#include <stddef.h>

/** Variant genotype.
 * @struct bgen_genotype
 */
struct bgen_genotype;

/** Close a variant genotype handler.
 *
 * @param vg Variant genotype handler.
 */
BGEN_EXPORT void bgen_genotype_close(struct bgen_genotype const* vg);
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
BGEN_EXPORT int bgen_read_genotype(struct bgen_file const* bgen, struct bgen_genotype* vg,
                                   double* p);
/** Get the number of alleles.
 *
 * @param vg Variant genotype handler.
 * @return Number of alleles.
 */
BGEN_EXPORT unsigned bgen_nalleles(struct bgen_genotype const* vg);
/** Return `1` if variant is missing for the sample; `0` otherwise.
 *
 * @param vg Variant genotype handler.
 * @param index Sample index.
 * @return `1` for missing genotype; `0` otherwise.
 */
BGEN_EXPORT bool bgen_missing(struct bgen_genotype const* vg, int index);
/** Get the ploidy.
 *
 * @param vg Variant genotype handler.
 * @param index Sample index.
 * @return Ploidy.
 */
BGEN_EXPORT unsigned bgen_ploidy(struct bgen_genotype const* vg, int index);
/** Get the minimum ploidy of the variant.
 *
 * @param vg Variant genotype handler.
 * @return Ploidy minimum.
 */
BGEN_EXPORT unsigned bgen_min_ploidy(struct bgen_genotype const* vg);
/** Get the maximum ploidy of the variant.
 *
 * @param vg Variant genotype handler.
 * @return Ploidy maximum.
 */
BGEN_EXPORT unsigned bgen_max_ploidy(struct bgen_genotype const* vg);
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
BGEN_EXPORT unsigned bgen_ncombs(struct bgen_genotype const* vg);
/** Return `1` for phased or `0` for unphased genotype.
 *
 * @param vg Variant genotype handler.
 * @return `1` for phased genotype; `0` otherwise.
 */
BGEN_EXPORT bool bgen_phased(struct bgen_genotype const* vg);

#endif
