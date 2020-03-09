#ifndef _BGEN_STR_H
#define _BGEN_STR_H

#include "bgen/bgen.h"
#include <stdio.h>

int fread_str(FILE *fp, struct bgen_str *s, size_t len_size);
int fwrite_str(FILE *fp, struct bgen_str const *s, size_t len_size);
void printf_str(struct bgen_str s);

#endif /* _BGEN_STR_H */
