#include "bgen/bgen.h"
#include "cass.h"

void create_metafile(char const* bgen_file_filepath, char const* metafile_filepath);
void use_metafile_properly(char const* metafile_filepath);
void use_metafile_wrongly(char const* metafile_filepath);

int main(void)
{
    char const bgen_file_filepath[] = TEST_DATADIR "complex.23bits.bgen";
    char const metafile_filepath[] = "index.tmp/complex_index03.metafile";

    create_metafile(bgen_file_filepath, metafile_filepath);
    use_metafile_properly(metafile_filepath);
    use_metafile_wrongly(metafile_filepath);

    return cass_status();
}

void create_metafile(char const* bgen_file_filepath, char const* metafile_filepath)
{
    struct bgen_file* bgen_file = bgen_file_open(bgen_file_filepath);
    cass_cond(bgen_file != NULL);
    struct bgen_metafile* metafile = bgen_metafile_create(bgen_file, metafile_filepath, 2, 0);
    cass_cond(metafile != NULL);
    cass_equal_int(bgen_metafile_close(metafile), 0);
    bgen_file_close(bgen_file);
}

void use_metafile_properly(char const* metafile_filepath)
{
    struct bgen_metafile* metafile = bgen_metafile_open(metafile_filepath);
    cass_cond(metafile != NULL);

    cass_equal_int(bgen_metafile_npartitions(metafile), 2);

    cass_equal_int(bgen_metafile_nvariants(metafile), 10);

    struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, 0);
    cass_cond(partition != NULL);

    struct bgen_variant const* vm = bgen_partition_get_variant(partition, 0);
    cass_cond(bgen_string_equal(BGEN_STRING(""), *vm->id));
    cass_cond(bgen_string_equal(BGEN_STRING("V1"), *vm->rsid));
    cass_cond(bgen_string_equal(BGEN_STRING("01"), *vm->chrom));
    cass_equal_int(vm[0].nalleles, 2);

    vm = bgen_partition_get_variant(partition, 1);
    cass_cond(bgen_string_equal(BGEN_STRING("A"), *vm->allele_ids[0]));
    cass_cond(bgen_string_equal(BGEN_STRING("G"), *vm->allele_ids[1]));

    vm = bgen_partition_get_variant(partition, 4);
    cass_cond(bgen_string_equal(BGEN_STRING(""), *vm->id));
    cass_cond(bgen_string_equal(BGEN_STRING("M5"), *vm->rsid));
    cass_cond(bgen_string_equal(BGEN_STRING("01"), *vm->chrom));
    cass_equal_int(vm->nalleles, 2);
    cass_cond(bgen_string_equal(BGEN_STRING("A"), *vm->allele_ids[0]));
    cass_cond(bgen_string_equal(BGEN_STRING("G"), *vm->allele_ids[1]));

    bgen_partition_destroy(partition);

    partition = bgen_metafile_read_partition(metafile, 1);
    cass_cond(partition != NULL);

    vm = bgen_partition_get_variant(partition, 0);
    cass_cond(bgen_string_equal(BGEN_STRING(""), *vm->id));
    cass_cond(bgen_string_equal(BGEN_STRING("M6"), *vm->rsid));
    cass_cond(bgen_string_equal(BGEN_STRING("01"), *vm->chrom));
    cass_equal_int(vm[0].nalleles, 4);

    vm = bgen_partition_get_variant(partition, 1);
    cass_cond(bgen_string_equal(BGEN_STRING("A"), *vm->allele_ids[0]));
    cass_cond(bgen_string_equal(BGEN_STRING("G"), *vm->allele_ids[1]));

    bgen_partition_destroy(partition);

    cass_equal_int(bgen_metafile_close(metafile), 0);
}

void use_metafile_wrongly(char const* metafile_filepath)
{
    struct bgen_metafile* metafile = bgen_metafile_open(metafile_filepath);
    cass_cond(metafile != NULL);
    cass_cond(bgen_metafile_read_partition(metafile, 3) == NULL);
    cass_equal_int(bgen_metafile_close(metafile), 0);
}
