#include "str.h"
#include "bgen/str.h"
#include "free.h"
#include "report.h"
#include <inttypes.h>

struct bgen_str const* bgen_str_fread(FILE* stream, size_t length_size)
{
    uint64_t length = 0;

    if (fread(&length, 1, length_size, stream) < length_size) {
        bgen_perror("error while freading a string length");
        return NULL;
    }

    if (length == 0)
        return bgen_str_create(NULL, 0);

    char* data = malloc(sizeof(char) * length);

    if (fread(data, 1, length, stream) < length) {
        if (ferror(stream))
            bgen_perror("error while freading a string str");
        else
            bgen_error("unexpected end of file while reading a string");
        free_c(data);
        return NULL;
    }

    return bgen_str_create(data, length);
}

int bgen_str_fwrite(struct bgen_str const* str, FILE* stream, size_t length_size)
{
    uint64_t len = str->length;
    if (fwrite(&len, length_size, 1, stream) != 1) {
        bgen_perror("error while fwriting a string length");
        return 1;
    }

    if (len == 0)
        return 0;

    if (fwrite(str->data, 1, len, stream) != len) {

        if (ferror(stream))
            bgen_perror("error while fwriting a string str");
        else
            bgen_error("unexpected end of file while writing a string");
        return 1;
    }
    return 0;
}
