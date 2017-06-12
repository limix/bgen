#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include <stdlib.h>
#include <string.h>


inline static size_t bit_to_byte_idx(size_t bi)
{
    return bi / 8;
}

inline static size_t bit_in_byte_idx(size_t bi)
{
    return bi % 8;
}

#endif /* end of include guard: UTIL_H */
