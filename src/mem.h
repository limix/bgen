#ifndef _BGEN_MEM_H
#define _BGEN_MEM_H

#include "io.h"
#include <stdlib.h>
#include <string.h>

static inline void *_dalloc(size_t size, const char *_file_, int _line_)
{
    void *p = malloc(size);
    if (p == NULL) {
        error("Error: could not allocate memory at %s, line %d.", _file_, _line_);
        return NULL;
    }
    return p;
}

static inline void *free_nul(void *p)
{
    if (p != NULL)
        free(p);
    return NULL;
}

#define dalloc(SIZE) _dalloc(SIZE, __FILE__, __LINE__)

static inline void bgen_memcpy(void *dst, char **src, size_t n)
{
    memcpy(dst, *src, n);
    *src = *src + n;
}

#endif /* _BGEN_MEM_H */
