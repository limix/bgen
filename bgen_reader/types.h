#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <assert.h>

typedef unsigned char byte;
typedef int_fast64_t  inti;
typedef double        real;

static_assert(sizeof(byte) == 1, "size of 'byte' should be 1 byte.");
static_assert(sizeof(inti) >= 8, "size of 'inti' should be at least 8 bytes.");
static_assert(sizeof(real) >= 8, "size of 'real' should be at least 8 bytes.");

#endif /* end of include guard: TYPES_H */
