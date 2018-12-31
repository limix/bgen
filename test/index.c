#include "bgen.h"
#include "cass.h"
#include "str.h"
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int create_index(const char *bgen_filepath, const char *filepath);
int use_index(const char *filepath);

int main()
{

    const char filepath[] = "complex_index03.index";
    const char bgen_filepath[] = "data/complex.23bits.bgen";

    if (create_index(bgen_filepath, filepath))
        return 1;

    if (use_index(filepath))
        return 1;

    return 0;
}

int create_index(const char *bgen_filepath, const char *filepath)
{
    struct bgen_file *bgen;

    assert_not_null(bgen = bgen_open(bgen_filepath));
    assert_not_null(bgen_create_metafile(bgen, filepath, 1));

    bgen_close(bgen);

    FILE *fp = fopen(filepath, "rb");
    assert_not_null(fp);

    char header[14];
    header[13] = '\0';
    assert_equal_int(fread(header, 13 * sizeof(char), 1, fp), 1);

    assert_strncmp(header, "bgen index 03", 13);

    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    assert_equal_int(fread(&u32, sizeof(uint32_t), 1, fp), 1);
    assert_equal_int(u32, 10);

    assert_equal_int(fread(&u64, sizeof(uint64_t), 1, fp), 1);
    assert_equal_uint64(u64, 484);

    fseek(fp, u64, SEEK_CUR);
    assert_equal_int(fread(&u32, sizeof(uint32_t), 1, fp), 1);
    assert_equal_int(u32, 2);

    assert_equal_int(fread(&u64, sizeof(uint64_t), 1, fp), 1);
    assert_equal_uint64(u64, 0);

    assert_equal_int(fread(&u64, sizeof(uint64_t), 1, fp), 1);
    assert_equal_uint64(u64, 179);

    fseek(fp, 13 + 4 + 8, SEEK_SET);
    assert_equal_int(fread(&u64, sizeof(uint64_t), 1, fp), 1);
    assert_equal_uint64(u64, 98);

    return 0;
}

int use_index(const char *filepath)
{
    struct bgen_mf *v;
    int nvariants;
    size_t i, j;

    assert_not_null(v = bgen_open_metafile(filepath));

    int nparts = bgen_metafile_nparts(v);
    assert_equal_int(nparts, 2);

    int nvars = bgen_metafile_nvars(v);
    assert_equal_int(nvars, 10);

    struct bgen_vm *vm = bgen_read_partition(v, 0, &nvariants);
    assert_not_null(vm);

    assert_equal_int(vm[0].id.len, 0);
    assert_equal_int(vm[0].rsid.len, 2);
    assert_strncmp(vm[0].rsid.str, "V1", 2);
    assert_strncmp(vm[0].chrom.str, "01", 2);
    assert_equal_int(vm[0].nalleles, 2);
    assert_strncmp(vm[1].allele_ids[0].str, "A", 1);
    assert_strncmp(vm[1].allele_ids[1].str, "G", 1);

    assert_equal_int(vm[4].id.len, 0);
    assert_strncmp(vm[4].rsid.str, "M5", 2);
    assert_strncmp(vm[4].chrom.str, "01", 2);
    assert_equal_int(vm[4].nalleles, 2);
    assert_strncmp(vm[4].allele_ids[0].str, "A", 1);
    assert_strncmp(vm[4].allele_ids[1].str, "G", 1);

    bgen_free_partition(vm, nvariants);

    vm = bgen_read_partition(v, 1, &nvariants);
    assert_not_null(vm);

    assert_equal_int(vm[0].id.len, 0);
    assert_equal_int(vm[0].rsid.len, 2);
    assert_strncmp(vm[0].rsid.str, "M6", 2);
    assert_strncmp(vm[0].chrom.str, "01", 2);
    assert_equal_int(vm[0].nalleles, 4);
    assert_strncmp(vm[1].allele_ids[0].str, "A", 1);
    assert_strncmp(vm[1].allele_ids[1].str, "G", 1);

    assert_equal_int(bgen_close_metafile(v), 0);

    return 0;
}
