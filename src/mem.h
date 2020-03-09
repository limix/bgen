#ifndef _BGEN_MEM_H
#define _BGEN_MEM_H

#include "io.h"
#include "report.h"
#include <stdlib.h>
#include <string.h>

static inline void memcpy_walk(void *dst, char **src, size_t n)
{
    memcpy(dst, *src, n);
    *src = *src + n;
}

#endif /* _BGEN_MEM_H */
