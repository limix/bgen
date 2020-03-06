#ifndef _BGEN_STR_H
#define _BGEN_STR_H

#include "bgen/bgen.h"
#include <stdio.h>

int fread_str(FILE *fp, struct bgen_str *s, size_t len_size);
int fwrite_str(FILE *fp, const struct bgen_str *s, size_t len_size);
void printf_str(struct bgen_str s);
void alloc_str(struct bgen_str *v, size_t len);
void free_str(struct bgen_str *v);

#endif /* _BGEN_STR_H */
