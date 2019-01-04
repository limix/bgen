#include "bgen.h"
#include "mem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void alloc_str(struct bgen_str *v, size_t len)
{
    v->len = len;
    if (len > 0)
        v->str = dalloc(len);
    else
        v->str = NULL;
}

void free_str(struct bgen_str *v)
{
    v->str = free_nul(v->str);
    v->len = 0;
}

int fread_str(FILE *fp, struct bgen_str *s, size_t len_size)
{
    uint64_t len = 0;

    if (fread(&len, 1, len_size, fp) < len_size) {
        perror("Error while freading a string length");
        return 1;
    }

    alloc_str(s, len);

    if (len == 0)
        return 0;

    if (fread(s->str, 1, s->len, fp) < (size_t)s->len) {
        if (ferror(fp))
            perror("Error while freading a string str");
        else
            error("Unexpected end of file while reading a string");
        return 1;
    }

    return 0;
}

int fwrite_str(FILE *fp, const struct bgen_str *s, size_t len_size)
{

    uint64_t len = s->len;
    if (fwrite(&len, len_size, 1, fp) != 1) {
        perror("Error while fwriting a string length");
        return 1;
    }

    if (len == 0)
        return 0;

    if (fwrite(s->str, 1, len, fp) != len) {

        if (ferror(fp))
            perror("Error while fwriting a string str");
        else
            error("Unexpected end of file while writing a string");
        return 1;
    }
    return 0;
}
