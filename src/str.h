#ifndef BGEN_UTIL_STR_H
#define BGEN_UTIL_STR_H

#include "bgen.h"
#include <stdio.h>

int fread_str(FILE *fp, struct bgen_str *s, size_t len_size);
int fwrite_str(FILE *fp, const struct bgen_str *s, size_t len_size);
void str_printf(struct bgen_str s);
void str_alloc(struct bgen_str *v, size_t len);
void str_free(struct bgen_str *v);

#endif /* end of include guard: BGEN_UTIL_STR_H */
