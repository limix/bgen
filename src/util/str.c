#include "bgen.h"

#include "util/stdint.h"
#include <stdio.h>
#include <stdlib.h>

void str_alloc(struct bgen_string *v, size_t len) {
    v->len = len;
    v->str = malloc(len + 1);
    v->str[len] = '\0';
}

void str_free(struct bgen_string *v) {
    free(v->str);
    v->str = NULL;
    v->len = 0;
}

int str_fread(FILE *fp, struct bgen_string *s, size_t len_size) {
    uint64_t len;
    len = 0;

    if (fread(&len, 1, len_size, fp) < len_size) {
        perror("Error while freading a string length ");
        return 1;
    }

    str_alloc(s, len);

    if (fread(s->str, 1, s->len, fp) < (size_t)s->len) {
        perror("Error while freading a string str ");
        return 1;
    }

    return 0;
}

int str_fwrite(FILE *fp, const struct bgen_string *s, size_t len_size) {

    uint64_t len = s->len;
    if (fwrite(&len, len_size, 1, fp) != 1) {
        perror("Error while fwriting a string length ");
        return 1;
    }

    if (fwrite(s->str, len * sizeof(char), 1, fp) != 1) {
        perror("Error while fwriting a string str ");
        return 1;
    }

    return 0;
}

void str_printf(struct bgen_string s) { printf("%.*s\n", s.len, s.str); }
