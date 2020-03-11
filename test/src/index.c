#include "bgen/bgen.h"
#include "cass.h"
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void use_metafile(struct bgen_mf* mf)
{
    int nvariants;

    int nparts = bgen_metafile_npartitions(mf);
    cass_equal_int(nparts, 2);

    int nvars = bgen_metafile_nvariants(mf);
    cass_equal_int(nvars, 10);

    struct bgen_partition* partition = bgen_metafile_read_partition2(mf, 0);
    cass_cond(partition != NULL);

    struct bgen_variant const* vm = bgen_partition_get(partition, 0);
    cass_cond(bgen_str_equal(BGEN_STR(""), *vm->id));
    cass_cond(bgen_str_equal(BGEN_STR("V1"), *vm->rsid));
    cass_cond(bgen_str_equal(BGEN_STR("01"), *vm->chrom));
    cass_equal_int(vm[0].nalleles, 2);

    vm = bgen_partition_get(partition, 1);
    cass_cond(bgen_str_equal(BGEN_STR("A"), *vm->allele_ids[0]));
    cass_cond(bgen_str_equal(BGEN_STR("G"), *vm->allele_ids[1]));

    vm = bgen_partition_get(partition, 4);
    cass_cond(bgen_str_equal(BGEN_STR(""), *vm->id));
    cass_cond(bgen_str_equal(BGEN_STR("M5"), *vm->rsid));
    cass_cond(bgen_str_equal(BGEN_STR("01"), *vm->chrom));
    cass_equal_int(vm->nalleles, 2);
    cass_cond(bgen_str_equal(BGEN_STR("A"), *vm->allele_ids[0]));
    cass_cond(bgen_str_equal(BGEN_STR("G"), *vm->allele_ids[1]));

    bgen_partition_destroy(partition);

    partition = bgen_metafile_read_partition2(mf, 1);
    cass_cond(partition != NULL);

    vm = bgen_partition_get(partition, 0);
    cass_cond(bgen_str_equal(BGEN_STR(""), *vm->id));
    cass_cond(bgen_str_equal(BGEN_STR("M6"), *vm->rsid));
    cass_cond(bgen_str_equal(BGEN_STR("01"), *vm->chrom));
    cass_equal_int(vm[0].nalleles, 4);

    vm = bgen_partition_get(partition, 1);
    cass_cond(bgen_str_equal(BGEN_STR("A"), *vm->allele_ids[0]));
    cass_cond(bgen_str_equal(BGEN_STR("G"), *vm->allele_ids[1]));

    bgen_partition_destroy(partition);
}

void use_metafile_wrongly(struct bgen_mf* mf)
{
    cass_cond(bgen_metafile_read_partition2(mf, 3) == NULL);
}

int main()
{

    const char mf_filepath[] = "complex_index03.metadata";
    const char bgen_filepath[] = "data/complex.23bits.bgen";

    struct bgen_file* bgen = bgen_file_open(bgen_filepath);
    cass_cond(bgen != NULL);
    struct bgen_mf* mf = bgen_metafile_create(bgen, mf_filepath, 2, 0);
    cass_cond(mf != NULL);
    cass_equal_int(bgen_metafile_close(mf), 0);

    cass_cond((mf = bgen_metafile_open(mf_filepath)) != NULL);
    use_metafile(mf);
    cass_equal_int(bgen_metafile_close(mf), 0);

    cass_cond((mf = bgen_metafile_open(mf_filepath)) != NULL);
    use_metafile_wrongly(mf);
    cass_equal_int(bgen_metafile_close(mf), 0);

    bgen_file_close(bgen);
    return cass_status();
}
