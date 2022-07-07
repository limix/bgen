#include "bgen/bgen.h"
#include "cass.h"

void test_zeo_len_chrom_id(void);

int main(void)
{
    test_zeo_len_chrom_id();
    return cass_status();
}

void test_zeo_len_chrom_id(void)
{
    struct bgen_file* bgen;

    bgen = bgen_file_open(TEST_DATADIR "zero_len_chrom_id.bgen");

    cass_cond(bgen != NULL);

    cass_cond(bgen_file_nsamples(bgen) == 182);
    cass_cond(bgen_file_nvariants(bgen) == 50);

    struct bgen_samples* samples = bgen_file_read_samples(bgen);
    cass_cond(samples != NULL);
    bgen_samples_destroy(samples);

    struct bgen_metafile* mf =
        bgen_metafile_create(bgen, "zero_len_chrom_id.tmp/zero_len_chrom_id.metafile", 2, 0);
    cass_cond(mf != NULL);

    struct bgen_partition const* partition = bgen_metafile_read_partition(mf, 0);
    struct bgen_variant const*   vm = bgen_partition_get_variant(partition, 0);
    cass_cond(vm != NULL);
    cass_cond(bgen_partition_nvariants(partition) == 25);
    cass_cond(bgen_string_equal(BGEN_STRING(""), *vm->chrom));
    bgen_partition_destroy(partition);

    bgen_metafile_close(mf);
    bgen_file_close(bgen);
}
