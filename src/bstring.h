#ifndef BGEN_BSTRING_H_PRIVATE
#define BGEN_BSTRING_H_PRIVATE

#include <stddef.h>
#include <stdio.h>

struct bgen_string;

struct bgen_string const* bgen_string_fread(FILE* restrict stream, size_t length_size);
struct bgen_string const* bgen_string_memfread(char const* restrict* src, size_t length_size);
int bgen_string_fwrite(struct bgen_string const* str, FILE* stream, size_t length_size);

#endif
