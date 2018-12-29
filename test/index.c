#include "bgen.h"
#include "str.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

#ifdef ASSERT
#undef ASSERT
#endif

void assertion_message(const char *file, int line, const char *msg) {
    fprintf(stderr, "Fail at %s:%d: %s\n", file, line, msg);
}

#define ASSERT(x)                                                                       \
    if (!(x)) {                                                                         \
        assertion_message(__FILE__, __LINE__, "'" #x "' should be TRUE");               \
        return 1;                                                                       \
    }

int create_index(const char *bgen_filepath, const char *filepath);
int use_index(const char *filepath);

int main() {

    const char filepath[] = "complex_index03.index";
    const char bgen_filepath[] = "data/complex.23bits.bgen";

    if (create_index(bgen_filepath, filepath))
        return 1;

    if (use_index(filepath))
        return 1;
    return 0;
}

int create_index(const char *bgen_filepath, const char *filepath) {
    struct bgen_file *bgen;

    ASSERT((bgen = bgen_open(bgen_filepath)) != NULL)

    ASSERT(bgen_create_metafile(bgen, filepath, 1) != 0)

    bgen_close(bgen);

    FILE *fp = fopen(filepath, "rb");

    char header[14];
    header[13] = '\0';
    fread(header, 13 * sizeof(char), 1, fp);

    ASSERT(strncmp(header, "bgen index 03", 13) == 0);

    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    fread(&u32, sizeof(uint32_t), 1, fp);
    ASSERT(u32 == 10);

    fread(&u64, sizeof(uint64_t), 1, fp);
    if (u64 != 484)
        return 1;

    fseek(fp, u64, SEEK_CUR);
    fread(&u32, sizeof(uint32_t), 1, fp);
    if (u32 != 2)
        return 1;

    fread(&u64, sizeof(uint64_t), 1, fp);
    if (u64 != 0)
        return 1;

    fread(&u64, sizeof(uint64_t), 1, fp);
    if (u64 != 179)
        return 1;

    fseek(fp, 13 + 4 + 8, SEEK_SET);
    fread(&u64, sizeof(uint64_t), 1, fp);
    if (u64 != 98)
        return 1;

    fclose(fp);
    return 0;
}

int use_index(const char *filepath) {
    struct bgen_mf *v;
    int nvariants;
    size_t i, j;

    if ((v = bgen_open_metafile(filepath)) == NULL) {
        return 1;
    }

    int nparts = bgen_metafile_nparts(v);
    if (nparts != 2)
        return 1;

    int nvars = bgen_metafile_nvars(v);
    if (nvars != 10)
        return 1;

    struct bgen_vm *vm = bgen_read_partition(v, 0, &nvariants);

    ASSERT(vm[0].id.len == 0);
    ASSERT(vm[0].rsid.len == 2);
    ASSERT(strncmp(vm[0].rsid.str, "V1", 2) == 0);
    ASSERT(strncmp(vm[0].chrom.str, "01", 2) == 0);
    ASSERT(vm[0].nalleles == 2);
    ASSERT(strncmp(vm[1].allele_ids[0].str, "A", 1) == 0);
    ASSERT(strncmp(vm[1].allele_ids[1].str, "G", 1) == 0);

    ASSERT(vm[4].id.len == 0);
    ASSERT(strncmp(vm[4].rsid.str, "M5", 2) == 0);
    ASSERT(strncmp(vm[4].chrom.str, "01", 2) == 0);
    ASSERT(vm[4].nalleles == 2);
    ASSERT(strncmp(vm[4].allele_ids[0].str, "A", 1) == 0);
    ASSERT(strncmp(vm[4].allele_ids[1].str, "G", 1) == 0);

    if (bgen_close_metafile(v))
        return 1;
    return 0;
}
