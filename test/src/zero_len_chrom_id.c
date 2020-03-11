#include "bgen/bgen.h"
#include "cass.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_zeo_len_chrom_id(void);

int main(void)
{
    test_zeo_len_chrom_id();
    return cass_status();
}

void test_zeo_len_chrom_id(void)
{
    struct bgen_file *bgen;

    bgen = bgen_file_open("data/zero_len_chrom_id.bgen");

    cass_cond(bgen != NULL);

    cass_cond(bgen_file_nsamples(bgen) == 182);
    cass_cond(bgen_file_nvariants(bgen) == 50);

    struct bgen_samples *samples = bgen_file_read_samples(bgen, 0);
    cass_cond(samples != NULL);
    bgen_samples_free(samples);

    struct bgen_mf *mf = bgen_metafile_create(bgen, "zero_len_chrom_id.metadata", 2, 0);
    cass_cond(mf != NULL);

    int nvariants = 0;
    struct bgen_partition* partition = bgen_metafile_read_partition2(mf, 0);
    struct bgen_variant_metadata const *vm = bgen_partition_get(partition, 0);
    cass_cond(vm != NULL);
    cass_cond(bgen_partition_nvariants(partition) == 25);
    cass_cond(bgen_str_equal(BGEN_STR(""), *vm->chrom));
    bgen_partition_destroy(partition);

    bgen_mf_close(mf);
    bgen_file_close(bgen);
}
