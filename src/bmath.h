#ifndef BGEN_BMATH_H
#define BGEN_BMATH_H

#include "report.h"
#include <inttypes.h>
#include <limits.h>

static inline uint32_t panic_add_uint32(uint32_t a, uint32_t b)
{
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

static inline uint32_t min_uint32(uint32_t a, uint32_t b) { return a < b ? a : b; }
static inline uint32_t max_uint32(uint32_t a, uint32_t b) { return a > b ? a : b; }

/* Number of ways to choose `k` elements from a total of `n`. */
static inline unsigned choose(unsigned n, unsigned k)
{
    unsigned ans = 1;

    k = k > n - k ? n - k : k;
    unsigned j = 1;

    for (; j <= k; j++, n--) {
        if (n % j == 0) {
            ans *= n / j;
        } else if (ans % j == 0) {
            ans = ans / j * n;
        } else {
            ans = (ans * n) / j;
        }
    }
    return ans;
}

#endif
