#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    struct bgen_file* bgen_file = bgen_file_open(TEST_DATADIR "roundtrip1.bgen");

    cass_cond(bgen_file_nvariants(bgen_file) == 100);

    struct bgen_metafile* metafile = bgen_metafile_create(bgen_file,
                                                          "variant_position_overflow.tmp/"
                                                          "roundtrip1.bgen.metafile",
                                                          1, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, 0);
    struct bgen_variant const*   variant = bgen_partition_get_variant(partition, 99);

    cass_cond(variant->position == 3037630001);

    bgen_partition_destroy(partition);

    bgen_metafile_close(metafile);
    bgen_file_close(bgen_file);

    return cass_status();
}
