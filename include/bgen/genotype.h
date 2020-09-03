/** Open, query, and close a variant genotype.
 * @file bgen/genotype.h
 */
#ifndef BGEN_GENOTYPE_H
#define BGEN_GENOTYPE_H

#include "bgen/export.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

/** Variant genotype handler.
 * @struct bgen_genotype
 */
struct bgen_genotype;
struct bgen_metafile;

/** Close a variant genotype handler.
 *
 * @param genotype Variant genotype handler.
 */
BGEN_EXPORT void bgen_genotype_close(struct bgen_genotype const* genotype);
/** Read the probabilities of each possible genotype (64-bits).
 *
 * The length of this array is equal to the product of the values obtained by calling
 * the functions @ref bgen_file_nsamples and @ref bgen_genotype_ncombs.
 * \rst
 * .. seealso::
 *     Please, refer to the corresponding section **Probability data storage** of the
 *     |bgen format specification| for more information.
 * \endrst
 *
 * @param genotype Variant genotype handler.
 * @param probabilities Array of probabilities.
 * @return `0` if it succeeds; `1` otherwise.
 */
BGEN_EXPORT int bgen_genotype_read(struct bgen_genotype* genotype, double* probabilities);
/** Read the probabilities of each possible genotype (64-bits).
 *
 * The length of this array is equal to the product of the values obtained by calling
 * the functions @ref bgen_file_nsamples and @ref bgen_genotype_ncombs.
 * \rst
 * .. seealso::
 *     Please, refer to the corresponding section **Probability data storage** of the
 *     |bgen format specification| for more information.
 * \endrst
 *
 * @param genotype Variant genotype handler.
 * @param probabilities Array of probabilities.
 * @return `0` if it succeeds; `1` otherwise.
 */
BGEN_EXPORT int bgen_genotype_read64(struct bgen_genotype* genotype, double* probabilities);
/** Read the probabilities of each possible genotype (32-bits).
 *
 * The length of this array is equal to the product of the values obtained by calling
 * the functions @ref bgen_file_nsamples and @ref bgen_genotype_ncombs.
 * \rst
 * .. seealso::
 *     Please, refer to the corresponding section **Probability data storage** of the
 *     |bgen format specification| for more information.
 * \endrst
 *
 * @param genotype Variant genotype handler.
 * @param probabilities Array of probabilities.
 * @return `0` if it succeeds; `1` otherwise.
 */
BGEN_EXPORT int bgen_genotype_read32(struct bgen_genotype* genotype, float* probabilities);
/** Get the number of alleles.
 *
 * @param genotype Variant genotype handler.
 * @return Number of alleles.
 */
BGEN_EXPORT uint16_t bgen_genotype_nalleles(struct bgen_genotype const* genotype);
/** Return `1` if variant is missing for the sample; `0` otherwise.
 *
 * @param genotype Variant genotype handler.
 * @param index Sample index.
 * @return `1` for missing genotype; `0` otherwise.
 */
BGEN_EXPORT bool bgen_genotype_missing(struct bgen_genotype const* genotype, uint32_t index);
/** Get the ploidy.
 *
 * @param genotype Variant genotype handler.
 * @param index Sample index.
 * @return Ploidy.
 */
BGEN_EXPORT uint8_t bgen_genotype_ploidy(struct bgen_genotype const* genotype, uint32_t index);
/** Get the minimum ploidy of the variant.
 *
 * @param genotype Variant genotype handler.
 * @return Ploidy minimum.
 */
BGEN_EXPORT uint8_t bgen_genotype_min_ploidy(struct bgen_genotype const* genotype);
/** Get the maximum ploidy of the variant.
 *
 * @param genotype Variant genotype handler.
 * @return Ploidy maximum.
 */
BGEN_EXPORT uint8_t bgen_genotype_max_ploidy(struct bgen_genotype const* genotype);
/** Get the number of genotype combinations.
 *
 * Precisely, if the bgen file is of **Layout 1**, the number of combinations is always
 * equal to `3`. In the case of **Layout 2**, we have two options. For phased genotype,
 * the number of combinations is equal to the product of @ref bgen_genotype_nalleles with
 * @ref bgen_genotype_max_ploidy. For unphased genotype, let `n` and `m` be the values returned
 * by calling @ref bgen_genotype_nalleles and @ref bgen_genotype_max_ploidy. This function
 * returns the number of combinations `n-1` alleles can be selected from `n+m-1`, such that the
 * order of a selection does not matter.
 *
 * @param genotype Variant genotype handler.
 * @return Number of combinations.
 */
BGEN_EXPORT unsigned bgen_genotype_ncombs(struct bgen_genotype const* genotype);
/** Return `1` for phased or `0` for unphased genotype.
 *
 * @param genotype Variant genotype handler.
 * @return `1` for phased genotype; `0` otherwise.
 */
BGEN_EXPORT bool bgen_genotype_phased(struct bgen_genotype const* genotype);

#endif
