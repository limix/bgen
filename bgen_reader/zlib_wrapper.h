#ifndef ZLIB_WRAPPER_H
#define ZLIB_WRAPPER_H

#include <zlib.h>
#include <stdint.h>
#include <stdlib.h>

#include "byte.h"

inline static int64_t zlib_uncompress(const BYTE *src, size_t src_size,
                                      BYTE **dst, uint32_t *dst_size)
{
    uLongf z_dst_size = *dst_size;

    int err = uncompress(*dst, &z_dst_size, src, src_size);

    if (err != Z_OK)
    {
        printf("zlib failed to uncompress: %s.\n", zError(err));
        return EXIT_FAILURE;
    }

    *dst_size = z_dst_size;

    *dst = realloc(*dst, *dst_size);

    return EXIT_SUCCESS;
}

#endif /* end of include guard: ZLIB_WRAPPER_H */
