#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    struct bgen_file* bgen_file = bgen_file_open("data/roundtrip1.bgen");

    cass_cond(bgen_file_nvariants(bgen_file) == 100);

    struct bgen_metafile_04* metafile = bgen_metafile_create_04(bgen_file,
                                                          "create_metafile_04.tmp/"
                                                          "roundtrip1.bgen.metafile",
                                                          1, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition_04(metafile, 0);
    struct bgen_variant const*   variant = bgen_partition_get_variant(partition, 99);

    cass_cond(variant->position == 3037630001);

    bgen_partition_destroy(partition);

    bgen_metafile_close_04(metafile);
    bgen_file_close(bgen_file);

    return cass_status();
}
