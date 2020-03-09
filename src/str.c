#include "bgen/bgen.h"
#include "str.h"
#include "free.h"
#include "mem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static struct bgen_str const* bgen_str_create_empty(void);

void alloc_str(struct bgen_str* v, size_t len);

void alloc_str(struct bgen_str* v, size_t len)
{
    v->length = len;
    if (len > 0)
        v->data = dalloc(len);
    else
        v->data = NULL;
}

/* void bgen_str_alloc(struct bgen_str* bgen_str, const size_t length) */
/* { */
/*     bgen_str->str = malloc(sizeof(char) * length); */
/* } */

char const* bgen_str_data(struct bgen_str const* bgen_str) { return bgen_str->data; }

void bgen_str_free(struct bgen_str const* v) { free_c(v->data); }

size_t bgen_str_length(struct bgen_str const* bgen_str) { return bgen_str->length; }

bool bgen_str_equal(struct bgen_str a, struct bgen_str b)
{
    if (a.length == b.length)
        return strncmp(a.data, b.data, a.length) == 0;
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
        bgen_perror("Error while freading a string length");
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
            bgen_perror("Error while freading a string str");
        else
            bgen_error("Unexpected end of file while reading a string");
        return 1;
    }

    return 0;
}

struct bgen_str const* bgen_str_fread_create(FILE *stream, size_t length_size)
{
    uint64_t length = 0;

    if (fread(&length, 1, length_size, stream) < length_size) {
        bgen_perror("Error while freading a string length");
        return NULL;
    }
    
    if (length == 0)
        return bgen_str_create_empty();

    char *data = malloc(sizeof(char) * length);

    if (fread(data, 1, length, stream) < length) {
        if (ferror(stream))
            bgen_perror("Error while freading a string str");
        else
            bgen_error("Unexpected end of file while reading a string");
        free_c(data);
        return NULL;
    }

    return bgen_str_create(data, length);
}

int fwrite_str(FILE* fp, struct bgen_str const* s, size_t len_size)
{

    uint64_t len = s->length;
    if (fwrite(&len, len_size, 1, fp) != 1) {
        bgen_perror("Error while fwriting a string length");
        return 1;
    }

    if (len == 0)
        return 0;

    if (fwrite(s->data, 1, len, fp) != len) {

        if (ferror(fp))
            bgen_perror("Error while fwriting a string str");
        else
            bgen_error("Unexpected end of file while writing a string");
        return 1;
    }
    return 0;
}

int bgen_str_fwrite(struct bgen_str const* s, FILE* fp, size_t len_size)
{
    uint64_t len = s->length;
    if (fwrite(&len, len_size, 1, fp) != 1) {
        bgen_perror("Error while fwriting a string length");
        return 1;
    }

    if (len == 0)
        return 0;

    if (fwrite(s->data, 1, len, fp) != len) {

        if (ferror(fp))
            bgen_perror("Error while fwriting a string str");
        else
            bgen_error("Unexpected end of file while writing a string");
        return 1;
    }
    return 0;
}

struct bgen_str const* bgen_str_create(char const* data, size_t length)
{
    struct bgen_str *str = malloc(sizeof(struct bgen_str));
    str->data = data;
    str->length = length;
    return str;
}

static struct bgen_str const* bgen_str_create_empty(void)
{
    struct bgen_str *str = malloc(sizeof(struct bgen_str));
    str->length = 0;
    return str;
}
