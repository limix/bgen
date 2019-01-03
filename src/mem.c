#include "mem.h"

void bgen_memcpy(void *dst, char **src, size_t n)
{
    memcpy(dst, *src, n);
    *src = *src + n;
}
