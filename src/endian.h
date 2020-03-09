#ifndef BGEN_ENDIAN_H
#define BGEN_ENDIAN_H

#include <stdbool.h>

static inline bool is_little_endian(void)
{
    int num = 1;

    return *(char*)&num == 1;
}

#endif
