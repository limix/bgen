#include "zip/zstd_wrapper.h"

inti bgen_zstd_uncompress(const byte *src, inti src_size, byte **dst,
                          inti *dst_size) {
    size_t dSize = ZSTD_decompress(*dst, *dst_size, src, src_size);

    if (dSize != *dst_size) {
        fprintf(stderr, "error decoding: %s \n", ZSTD_getErrorName(dSize));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
