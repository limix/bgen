#include "zip/zstd_wrapper.h"

inti bgen_unzstd(const byte *src, inti src_size, byte **dst, inti *dst_size) {
    size_t dSize = ZSTD_decompress(*dst, *dst_size, src, src_size);

    if (dSize != *dst_size) {
        fprintf(stderr, "decoded %lld\n", dSize);
        fprintf(stderr, "error decoding: %s \n", ZSTD_getErrorName(dSize));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

byte *bgen_zstd(const byte *src, inti src_size, inti *dst_size) {

    byte *dst;
    inti dst_capacity;
    size_t dst_size_;

    dst_capacity = ZSTD_compressBound(src_size) + src_size;
    dst = malloc(dst_capacity);

    dst_size_ =
        ZSTD_compress(dst, dst_capacity, src, src_size, 5);

    if (ZSTD_isError(dst_size_)) {
        fprintf(stderr, "error encoding: %s \n", ZSTD_getErrorName(dst_size_));
        return NULL;
    }
    *dst_size = dst_size_;

    return realloc(dst, dst_size_);
}
