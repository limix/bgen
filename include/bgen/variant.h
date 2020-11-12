#ifndef BGEN_VARIANT_H
#define BGEN_VARIANT_H

#include <inttypes.h>

/** Variant metadata.
 * @struct bgen_variant
 */
struct bgen_variant
{
    uint64_t                   genotype_offset; /**< Genotype offset (bgen file). */
    struct bgen_string const*  id;              /**< Variant identification. */
    struct bgen_string const*  rsid;            /**< RSID. */
    struct bgen_string const*  chrom;           /**< Chromossome name. */
    uint32_t                   position;        /**< Base-pair position. */
    uint16_t                   nalleles;        /**< Number of alleles. */
    struct bgen_string const** allele_ids;      /**< Allele ids. */
};

#endif
