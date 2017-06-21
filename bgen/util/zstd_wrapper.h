#ifndef ZSTD_WRAPPER_H
#define ZSTD_WRAPPER_H

#include <zstd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../types.h"

inline static inti zstd_uncompress(const byte *src, inti src_size,
                                   byte **dst, inti *dst_size)
{
    size_t dSize = ZSTD_decompress(*dst, *dst_size, src, src_size);

    if (dSize != *dst_size) {
        fprintf(stderr, "error decoding: %s \n", ZSTD_getErrorName(dSize));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif /* end of include guard: ZSTD_WRAPPER_H */
