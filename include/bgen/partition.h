#ifndef BGEN_PARTITION_H
#define BGEN_PARTITION_H

#include "bgen/str.h"
#include "bgen_export.h"
#include <stdint.h>

/** Partition of variant metadata.
 * @struct bgen_partition
 */
struct bgen_partition;

struct bgen_variant_metadata;

BGEN_EXPORT void bgen_partition_destroy(struct bgen_partition const* partition);
BGEN_EXPORT struct bgen_variant_metadata const* bgen_partition_get(struct bgen_partition const* partition,
                                                     uint32_t                     index);

#endif
