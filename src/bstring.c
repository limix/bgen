#include "bstring.h"
#include "bgen/bstring.h"
#include "free.h"
#include "mem.h"
#include "report.h"
#include <inttypes.h>

struct bgen_string const* bgen_string_fread(FILE* restrict stream, size_t length_size)
{
    uint64_t length = 0;

    if (fread(&length, 1, length_size, stream) < length_size)
        return NULL;

    if (length == 0)
        return bgen_string_create(NULL, 0);

    char* data = malloc(sizeof(char) * length);

    if (fread(data, 1, length, stream) < length) {
        bgen_free(data);
        return NULL;
    }

    return bgen_string_create(data, length);
}

struct bgen_string const* bgen_string_memfread(char const* restrict* src, size_t length_size)
{
    uint64_t length = 0;

    bgen_memfread(&length, src, length_size);

    if (length == 0)
        return bgen_string_create(NULL, 0);

    char* data = malloc(sizeof(char) * length);

    bgen_memfread(data, src, length);

    return bgen_string_create(data, length);
}

int bgen_string_fwrite(struct bgen_string const* str, FILE* stream, size_t length_size)
{
    uint64_t len = str->length;
    if (fwrite(&len, length_size, 1, stream) != 1)
        return 1;

    if (len == 0)
        return 0;

    if (fwrite(str->data, 1, len, stream) != len)
        return 1;

    return 0;
}
