#include "zip/zstd.h"
#include "report.h"
#include <stdlib.h>
#include <zstd.h>

int bgen_unzstd(const char* src, size_t src_size, void** dst, size_t* dst_size)
{
    size_t dSize = ZSTD_decompress(*dst, *dst_size, src, src_size);

    if (ZSTD_isError(dSize)) {
        bgen_error("zstd decoding: %s", ZSTD_getErrorName(dSize));
        return 1;
    }

    return 0;
}

char* bgen_zstd(const char* src, size_t src_size, size_t* dst_size)
{
    char*  dst;
    size_t dst_capacity;
    size_t dst_size_;

    dst_capacity = ZSTD_compressBound(src_size) + src_size;
    dst = malloc(dst_capacity);

    dst_size_ = ZSTD_compress(dst, dst_capacity, src, src_size, 5);

    if (ZSTD_isError(dst_size_)) {
        bgen_error("zstd encoding: %s", ZSTD_getErrorName(dst_size_));
        return NULL;
    }
    *dst_size = dst_size_;

    return realloc(dst, dst_size_);
}
