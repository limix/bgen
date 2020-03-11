#ifndef BGEN_STR_H_PRIVATE
#define BGEN_STR_H_PRIVATE

#include <stdio.h>

struct bgen_str;

struct bgen_str const* bgen_str_fread(FILE* stream, size_t length_size);
int bgen_str_fwrite(struct bgen_str const* str, FILE* stream, size_t length_size);

#endif
