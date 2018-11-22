#ifndef MEM_H
#define MEM_H

#include "util/print.h"
#include <stdlib.h>
#include <string.h>

static inline void *mem_alloc(size_t size, const char *_file_, int _line_) {
    void *p = malloc(size);
    if (p == NULL) {

        error("Error: could not allocate memory at %s, line %d.", _file_, _line_);
        return NULL;
    }

    return p;
}

static inline void *mem_free(void *p) {
    if (p != NULL)
        free(p);
    return NULL;
}

#define ALLOC(SIZE) mem_alloc(SIZE, __FILE__, __LINE__)
#define FREE(P) mem_free(P)

// Duplicate a string.
char *bgen_strdup(const char *src);
void bgen_memcpy(void *dst, char **src, size_t n);

#endif
