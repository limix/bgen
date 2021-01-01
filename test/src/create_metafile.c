#include "bgen/bgen.h"
#include "cass.h"

void test_1_partition(void);
void test_2_partition(void);
void test_3_partition(void);
void test_100_partition(void);

int main(void)
{
    test_1_partition();
    test_2_partition();
    test_3_partition();
    test_100_partition();
    return cass_status();
}

void test_1_partition(void)
{
    struct bgen_file* bgen_file = bgen_file_open(TEST_DATADIR "roundtrip1.bgen");

    cass_cond(bgen_file_nvariants(bgen_file) == 100);

    struct bgen_metafile* metafile = bgen_metafile_create(bgen_file,
                                                          "one_million.tmp/"
                                                          "roundtrip1.bgen.metafile1",
                                                          1, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, 0);
    struct bgen_variant const*   variant = bgen_partition_get_variant(partition, 99);

    cass_cond(variant->position == 3037630001);

    bgen_partition_destroy(partition);
    bgen_metafile_close(metafile);
    bgen_file_close(bgen_file);
}

void test_2_partition(void)
{
    struct bgen_file* bgen_file = bgen_file_open(TEST_DATADIR "roundtrip1.bgen");

    cass_cond(bgen_file_nvariants(bgen_file) == 100);

    struct bgen_metafile* metafile = bgen_metafile_create(bgen_file,
                                                          "one_million.tmp/"
                                                          "roundtrip1.bgen.metafile2",
                                                          2, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, 0);
    struct bgen_variant const*   variant = bgen_partition_get_variant(partition, 24);

    cass_cond(variant->position == 762630001);

    bgen_partition_destroy(partition);
    bgen_metafile_close(metafile);
    bgen_file_close(bgen_file);
}

void test_3_partition(void)
{
    struct bgen_file* bgen_file = bgen_file_open(TEST_DATADIR "roundtrip1.bgen");

    cass_cond(bgen_file_nvariants(bgen_file) == 100);

    struct bgen_metafile* metafile = bgen_metafile_create(bgen_file,
                                                          "one_million.tmp/"
                                                          "roundtrip1.bgen.metafile3",
                                                          3, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, 2);
    cass_cond(bgen_partition_nvariants(partition) == 32);

    struct bgen_variant const* variant = bgen_partition_get_variant(partition, 31);
    cass_cond(variant->position == 3037630001);

    bgen_partition_destroy(partition);
    bgen_metafile_close(metafile);
    bgen_file_close(bgen_file);
}

void test_100_partition(void)
{
    struct bgen_file* bgen_file = bgen_file_open(TEST_DATADIR "roundtrip1.bgen");

    cass_cond(bgen_file_nvariants(bgen_file) == 100);

    struct bgen_metafile* metafile = bgen_metafile_create(bgen_file,
                                                          "one_million.tmp/"
                                                          "roundtrip1.bgen.metafile100",
                                                          100, 0);

    struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, 99);
    struct bgen_variant const*   variant = bgen_partition_get_variant(partition, 0);

    cass_cond(variant->position == 3037630001);

    bgen_partition_destroy(partition);
    bgen_metafile_close(metafile);
    bgen_file_close(bgen_file);
}
