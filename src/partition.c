#include "partition.h"
#include "free.h"
#include "str.h"
#include "variant_metadata.h"

struct bgen_partition
{
    struct bgen_variant_metadata const** variants_metadata;
    uint32_t                             nvariants;
};

void bgen_partition_destroy(struct bgen_partition const* partition)
{
    if (partition->variants_metadata) {
        for (uint32_t i = 0; i < partition->nvariants; ++i) {
            if (partition->variants_metadata[i])
                bgen_variant_metadata_destroy(partition->variants_metadata[i]);
        }
        free_c(partition->variants_metadata);
    }

    free_c(partition);
}

struct bgen_variant_metadata const* bgen_partition_get(struct bgen_partition const* partition,
                                                       uint32_t const               index)
{
    return partition->variants_metadata[index];
}

uint32_t bgen_partition_nvariants(struct bgen_partition const* partition)
{
    return partition->nvariants;
}

struct bgen_partition* bgen_partition_create(uint32_t const nvariants)
{
    struct bgen_partition* partition = malloc(sizeof(struct bgen_partition));
    partition->variants_metadata = malloc(sizeof(struct bgen_variant_metadata*) * nvariants);
    partition->nvariants = nvariants;
    for (uint32_t i = 0; i < nvariants; ++i)
        partition->variants_metadata[i] = NULL;
    return partition;
}

void bgen_partition_set(struct bgen_partition* partition, uint32_t const index,
                        struct bgen_variant_metadata const* variant_metadata)
{
    partition->variants_metadata[index] = variant_metadata;
}
