#ifndef BGEN_BMATH_H
#define BGEN_BMATH_H

#include <limits.h>
#include <inttypes.h>
#include "report.h"

static inline uint32_t panic_add_uint32(uint32_t a, uint32_t b) {
    if (b > UINT32_MAX - a) {
        bgen_die("panic_add_uint32 overflow");
    }
    return a + b;
}

static inline uint32_t ceildiv_uint32(uint32_t x, uint32_t y)
{
    if (y == 0)
        bgen_die("y cannot be zero in ceildiv");
    return (panic_add_uint32(x, y - 1)) / y;
}

#endif
