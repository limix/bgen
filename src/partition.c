#include "partition.h"
#include "bstring.h"
#include "free.h"
#include "variant.h"

struct bgen_partition
{
    struct bgen_variant const** variants_metadata;
    uint32_t                    nvariants;
};

void bgen_partition_destroy(struct bgen_partition const* partition)
{
    if (partition->variants_metadata) {
        for (uint32_t i = 0; i < partition->nvariants; ++i) {
            if (partition->variants_metadata[i])
                bgen_variant_destroy(partition->variants_metadata[i]);
        }
        bgen_free(partition->variants_metadata);
    }

    bgen_free(partition);
}

struct bgen_variant const* bgen_partition_get_variant(struct bgen_partition const* partition,
                                                      uint32_t                     index)
{
    return partition->variants_metadata[index];
}

uint32_t bgen_partition_nvariants(struct bgen_partition const* partition)
{
    return partition->nvariants;
}

struct bgen_partition* bgen_partition_create(uint32_t nvariants)
{
    struct bgen_partition* partition = malloc(sizeof(struct bgen_partition));
    partition->variants_metadata = malloc(sizeof(struct bgen_variant*) * nvariants);
    partition->nvariants = nvariants;
    for (uint32_t i = 0; i < nvariants; ++i)
        partition->variants_metadata[i] = NULL;
    return partition;
}

void bgen_partition_set(struct bgen_partition* partition, uint32_t index,
                        struct bgen_variant const* variant_metadata)
{
    partition->variants_metadata[index] = variant_metadata;
}
