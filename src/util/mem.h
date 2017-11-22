#ifndef BGEN_UTIL_MEM_H
#define BGEN_UTIL_MEM_H

#include <stdlib.h>
#include <string.h>

// Duplicate a string.
char *bgen_strdup(const char *src);
void bgen_memcpy(void *dst, char **src, size_t n);

#endif /* end of include guard: BGEN_UTIL_MEM_H */
