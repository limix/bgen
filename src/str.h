#ifndef BGEN_STR_H_PRIVATE
#define BGEN_STR_H_PRIVATE

#include "bgen/str.h"
#include <stdio.h>

int fread_str(FILE* fp, struct bgen_str* s, size_t len_size);
int fwrite_str(FILE* fp, struct bgen_str const* s, size_t len_size);
void printf_str(struct bgen_str s);
struct bgen_str const* bgen_str_fread_create(FILE* stream, size_t length_size);
int bgen_str_fwrite(struct bgen_str const* s, FILE* fp, size_t len_size);

#endif
