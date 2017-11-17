#ifndef BGEN_UTIL_MEM_H
#define BGEN_UTIL_MEM_H

#include <stdlib.h>
#include <string.h>

#include "bgen/number.h"

// Duplicate a string.
byte *bgen_strdup(const byte *src);
void bgen_memcpy(void *dst, byte **src, inti n);

#endif /* end of include guard: BGEN_UTIL_MEM_H */
