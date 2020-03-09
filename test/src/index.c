#include "bgen/bgen.h"
#include "cass.h"
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void use_metafile(struct bgen_mf *mf)
{
    int nvariants;

    int nparts = bgen_metafile_npartitions(mf);
    cass_equal_int(nparts, 2);

    int nvars = bgen_metafile_nvariants(mf);
    cass_equal_int(nvars, 10);

    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvariants);
    cass_cond(vm != NULL);

    cass_equal_int(vm[0].id.len, 0);
    cass_equal_int(vm[0].rsid.len, 2);
    cass_cond(bgen_str_equal(BGEN_STR("V1"), vm[0].rsid));
    cass_cond(bgen_str_equal(BGEN_STR("01"), vm[0].chrom));
    cass_equal_int(vm[0].nalleles, 2);
    cass_cond(bgen_str_equal(BGEN_STR("A"), vm[1].allele_ids[0]));
    cass_cond(bgen_str_equal(BGEN_STR("G"), vm[1].allele_ids[1]));

    cass_equal_int(vm[4].id.len, 0);
    cass_cond(bgen_str_equal(BGEN_STR("M5"), vm[4].rsid));
    cass_cond(bgen_str_equal(BGEN_STR("01"), vm[4].chrom));
    cass_equal_int(vm[4].nalleles, 2);
    cass_cond(bgen_str_equal(BGEN_STR("A"), vm[4].allele_ids[0]));
    cass_cond(bgen_str_equal(BGEN_STR("G"), vm[4].allele_ids[1]));

    bgen_free_partition(vm, nvariants);

    vm = bgen_read_partition(mf, 1, &nvariants);
    cass_cond(vm != NULL);

    cass_equal_int(vm[0].id.len, 0);
    cass_equal_int(vm[0].rsid.len, 2);
    cass_cond(bgen_str_equal(BGEN_STR("M6"), vm[0].rsid));
    cass_cond(bgen_str_equal(BGEN_STR("01"), vm[0].chrom));
    cass_equal_int(vm[0].nalleles, 4);
    cass_cond(bgen_str_equal(BGEN_STR("A"), vm[1].allele_ids[0]));
    cass_cond(bgen_str_equal(BGEN_STR("G"), vm[1].allele_ids[1]));

    bgen_free_partition(vm, nvariants);
}

void use_metafile_wrongly(struct bgen_mf *mf)
{
    int nvariants;
    bgen_metafile_npartitions(mf);
    struct bgen_vm *vm = bgen_read_partition(mf, 3, &nvariants);
    cass_cond(vm == NULL);
}

int main()
{

    const char mf_filepath[] = "complex_index03.metadata";
    const char bgen_filepath[] = "data/complex.23bits.bgen";

    struct bgen_file *bgen = bgen_file_open(bgen_filepath);
    cass_cond(bgen != NULL);
    struct bgen_mf *mf = bgen_create_metafile(bgen, mf_filepath, 2, 0);
    cass_cond(mf != NULL);
    cass_equal_int(bgen_close_metafile(mf), 0);

    cass_cond((mf = bgen_open_metafile(mf_filepath)) != NULL);
    use_metafile(mf);
    cass_equal_int(bgen_close_metafile(mf), 0);

    cass_cond((mf = bgen_open_metafile(mf_filepath)) != NULL);
    use_metafile_wrongly(mf);
    cass_equal_int(bgen_close_metafile(mf), 0);

    bgen_file_close(bgen);
    return cass_status();
}
