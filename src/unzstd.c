#include "unzstd.h"
#include "report.h"
#include "zstd.h"

int bgen_unzstd(char const* src, size_t src_size, void** dst, size_t* dst_size)
{
    size_t dSize = ZSTD_decompress(*dst, *dst_size, src, src_size);

    if (ZSTD_isError(dSize)) {
        bgen_error("zstd decoding (%s)", ZSTD_getErrorName(dSize));
        return 1;
    }

    return 0;
}
