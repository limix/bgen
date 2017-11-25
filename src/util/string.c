#include "bgen/string.h"

#include <stdlib.h>
#include <stdio.h>
#include "util/stdint.h"

void bgen_string_alloc(string *v, size_t len) {
    v->len = len;
    v->str = malloc(len + 1);
    v->str[len] = '\0';
}

int fread_string(FILE *file, string *s, size_t len_size) {
    uint32_t len;
    len = 0;

    if (fread(&len, 1, len_size, file) < len_size){
        perror("Error while freading a string ");
        return 1;
    }

    bgen_string_alloc(s, len);

    if (fread(s->str, 1, s->len, file) < (size_t) s->len) {
        perror("Error while freading a string ");
        return 1;
    }

    return 0;
}
