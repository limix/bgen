#ifndef BGEN_FREE_H
#define BGEN_FREE_H

#include <stdlib.h>

static inline void free_c(void const* p) { free((void*)p); }

#endif
