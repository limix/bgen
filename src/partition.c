#include "partition.h"
#include "free.h"
#include "str.h"
#include "variant_metadata.h"

struct bgen_partition
{
    struct bgen_vm const** variants_metadata;
    uint32_t               nvariants;
};

void bgen_partition_destroy(struct bgen_partition const* partition)
{
    for (uint32_t i = 0; i < partition->nvariants; ++i) {
        if (partition->variants_metadata[i])
            bgen_variant_metadata_destroy(partition->variants_metadata[i]);
    }

    free_c(partition->variants_metadata);
    free_c(partition);
}

struct bgen_vm const* bgen_partition_get(struct bgen_partition const* partition,
                                         uint32_t const               index)
{
    return partition->variants_metadata[index];
}

struct bgen_partition* bgen_partition_create(uint32_t const nvariants)
{
    struct bgen_partition* partition = malloc(sizeof(struct bgen_partition));
    partition->variants_metadata = malloc(sizeof(struct bgen_vm*) * nvariants);
    partition->nvariants = nvariants;
    for (uint32_t i = 0; i < nvariants; ++i)
        partition->variants_metadata[i] = NULL;
    return partition;
}

void bgen_partition_set(struct bgen_partition* partition, uint32_t const index,
                        struct bgen_vm const* variant_metadata)
{
    partition->variants_metadata[index] = variant_metadata;
}
