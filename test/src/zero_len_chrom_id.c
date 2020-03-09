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
    struct bgen_str *samples;

    bgen = bgen_file_open("data/zero_len_chrom_id.bgen");

    cass_cond(bgen != NULL);

    cass_cond(bgen_file_nsamples(bgen) == 182);
    cass_cond(bgen_file_nvariants(bgen) == 50);

    samples = bgen_file_read_samples(bgen, 0);
    cass_cond(samples != NULL);
    free(samples);

    struct bgen_mf *mf = bgen_create_metafile(bgen, "zero_len_chrom_id.metadata", 2, 0);
    cass_cond(mf != NULL);

    int nvariants = 0;
    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvariants);
    cass_cond(vm != NULL);
    cass_cond(nvariants == 25);
    cass_cond(vm[0].chrom.length == 0);
    bgen_free_partition(vm, nvariants);

    bgen_close_metafile(mf);
    bgen_file_close(bgen);
}
