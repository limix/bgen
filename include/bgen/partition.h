/** Partition of variants.
 * @file bgen/partition.h
 */
#ifndef BGEN_PARTITION_H
#define BGEN_PARTITION_H

#include "bgen_export.h"
#include <stdint.h>

/** Partition of variants metadata.
 * @struct bgen_partition
 */
struct bgen_partition;
struct bgen_variant;

/** Destroy a partition.
 *
 * @param partition Partition of variants metadata.
 */
BGEN_EXPORT void bgen_partition_destroy(struct bgen_partition const* partition);
/** Get metadata from a specific variant.
 *
 * @param partition Partition of variants metadata.
 * @param index Variant index.
 * @return Variant metadata.
 */
BGEN_EXPORT struct bgen_variant const* bgen_partition_get(
    struct bgen_partition const* partition, uint32_t index);
/** Get the number of variants.
 *
 * @param partition Partition of variants metadata.
 * @return Number of variants.
 */
BGEN_EXPORT uint32_t bgen_partition_nvariants(struct bgen_partition const* partition);

#endif
