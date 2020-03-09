#include "bgen/bgen.h"
#include "free.h"
#include "mem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void alloc_str(struct bgen_str* v, size_t len);

void alloc_str(struct bgen_str* v, size_t len)
{
    v->len = len;
    if (len > 0)
        v->str = dalloc(len);
    else
        v->str = NULL;
}

/* void bgen_str_alloc(struct bgen_str* bgen_str, const size_t length) */
/* { */
/*     bgen_str->str = malloc(sizeof(char) * length); */
/* } */

char const* bgen_str_data(struct bgen_str const* bgen_str) { return bgen_str->str; }

void bgen_str_free(struct bgen_str const* v) { free_c(v->str); }

size_t bgen_str_length(struct bgen_str const* bgen_str) { return bgen_str->len; }

bool bgen_str_equal(struct bgen_str a, struct bgen_str b)
{
    if (a.len == b.len)
        return strncmp(a.str, b.str, a.len) == 0;
    return 0;
}

struct bgen_str BGEN_STR(char const* str)
{
    return (struct bgen_str){strlen(str), str};
}

int fread_str(FILE *fp, struct bgen_str *bgen_str, size_t len_size)
{
    uint64_t len = 0;

    if (fread(&len, 1, len_size, fp) < len_size) {
        perror("Error while freading a string length");
        return 1;
    }

    /* bgen_str_alloc(bgen_str, len); */
    alloc_str(bgen_str, len);

    if (len == 0)
        return 0;

    char* data = bgen_str_data(bgen_str);
    size_t length = bgen_str_length(bgen_str);
    if (fread(data, 1, length, fp) < length) {
        if (ferror(fp))
            perror("Error while freading a string str");
        else
            error("Unexpected end of file while reading a string");
        return 1;
    }

    return 0;
}


int fwrite_str(FILE* fp, struct bgen_str const* s, size_t len_size)
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
