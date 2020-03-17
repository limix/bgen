#ifndef BGEN_MEM_H
#define BGEN_MEM_H

#include <string.h>

static inline void memcpy_walk(void* dst, char** src, size_t n)
{
    memcpy(dst, *src, n);
    *src = *src + n;
}

static inline void bgen_memfread(void* restrict dst, char const* restrict* src, size_t n)
{
    memcpy(dst, *src, n);
    *src = *src + n;
}

#endif
