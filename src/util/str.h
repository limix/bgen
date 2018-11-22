#ifndef BGEN_UTIL_STR_H
#define BGEN_UTIL_STR_H

#include "bgen.h"
#include <stdio.h>

int str_fread(FILE *fp, struct bgen_string *s, size_t len_size);
int str_fwrite(FILE *fp, const struct bgen_string *s, size_t len_size);
void str_printf(struct bgen_string s);
void str_alloc(struct bgen_string *v, size_t len);
void str_free(struct bgen_string *v);

#endif /* end of include guard: BGEN_UTIL_STR_H */
