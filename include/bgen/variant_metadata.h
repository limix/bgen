#ifndef BGEN_VARIANT_METADATA_H
#define BGEN_VARIANT_METADATA_H

#include <inttypes.h>

/** Variant metadata.
 * @struct bgen_vm
 */
struct bgen_vm
{
    long                    vaddr;      /**< Variant address. */
    struct bgen_str const*  id;         /**< Variant identification. */
    struct bgen_str const*  rsid;       /**< RSID. */
    struct bgen_str const*  chrom;      /**< Chromossome name. */
    uint32_t                position;   /**< Base-pair position. */
    uint16_t                nalleles;   /**< Number of alleles. */
    struct bgen_str const** allele_ids; /**< Allele ids. */
};

#endif
