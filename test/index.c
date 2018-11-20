#include "bgen.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

int main() {

    const char filepath[] = "complex_index03.index";
    const char bgen_filepath[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;

    if ((bgen = bgen_open(bgen_filepath)) == NULL)
        return 1;

    if (bgen_create_index_file(bgen, filepath, 1))
        return 1;

    bgen_close(bgen);

    FILE *fp = fopen(filepath, "rb");

    char header[14];
    header[13] = '\0';
    fread(header, 13 * sizeof(char), 1, fp);

    if (strncmp(header, "bgen index 03", 13))
        return 1;

    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    fread(&u32, sizeof(uint32_t), 1, fp);
    if (u32 != 10)
        return 1;

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

    fclose(fp);

    return 0;
}
