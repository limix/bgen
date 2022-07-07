#define ZLIB_CONST
#include "unzlib.h"
#include "report.h"
#include "zlib.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int bgen_unzlib(char const* src, size_t src_size, char** dst, size_t* dst_size)
{
    z_stream strm = {
        .zalloc = Z_NULL,
        .zfree = Z_NULL,
        .opaque = Z_NULL,
        .avail_in = 0,
        .next_in = (unsigned char const*)src,
    };

    int e = inflateInit(&strm);

    if (e != Z_OK) {
        bgen_error("zlib failed to init (%s)", zError(e));
        goto err;
    }

    if (src_size > UINT_MAX) {
        bgen_error("zlib src_size overflow");
        goto err;
    }
    strm.avail_in = (unsigned)src_size;

    if (*dst_size > UINT_MAX) {
        bgen_error("zlib *dst_size overflow");
        goto err;
    }
    strm.avail_out = (unsigned)*dst_size;
    strm.next_out = (unsigned char*)*dst;

    e = inflate(&strm, Z_FINISH);
    if (e != Z_STREAM_END) {
        bgen_error("zlib failed to inflate (%s)", zError(e));
        goto err;
    }

    if (inflateEnd(&strm) != Z_OK) {
        bgen_error("zlib failed to inflateEnd (%s)", zError(e));
        return 1;
    }
    return 0;

err:
    inflateEnd(&strm);
    return 1;
}

int bgen_unzlib_chunked(char const* src, size_t src_size, char** dst, size_t* dst_size)
{
    if (*dst_size > UINT_MAX) {
        bgen_error("zlib *dst_size overflow");
        return 1;
    }

    unsigned       unused = (unsigned)*dst_size;
    unsigned char* cdst = (unsigned char*)*dst;

    z_stream strm = {
        .zalloc = Z_NULL, .zfree = Z_NULL, .opaque = Z_NULL, .avail_in = 0, .next_in = Z_NULL};

    int ret = inflateInit(&strm);

    if (ret != Z_OK) {
        bgen_error("zlib failed to uncompress (%s)", zError(ret));
        goto err;
    }

    strm.avail_in = (unsigned)src_size;
    strm.next_in = (unsigned char const*)src;

    while (1) {
        strm.avail_out = unused;
        strm.next_out = cdst;

        ret = inflate(&strm, Z_NO_FLUSH);

        if (ret == Z_NEED_DICT) {
            ret = Z_DATA_ERROR;
            goto err;
        }

        if (ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
            goto err;

        unsigned just_wrote = unused - strm.avail_out;

        cdst += just_wrote;
        unused -= just_wrote;

        if (ret == Z_STREAM_END) {
            *dst_size -= unused;
            *dst = (char*)realloc(*dst, *dst_size);
            break;
        }

        if (strm.avail_out == 0) {
            if (unused > 0) {
                bgen_error("zlib failed to uncompress (unknown error)");
                goto err;
            }

            unused = (unsigned)*dst_size;
            *dst_size += unused;
            *dst = (char*)realloc(*dst, *dst_size);
            cdst = (unsigned char*)*dst + unused;
        }
    }

    inflateEnd(&strm);
    return 0;

err:
    inflateEnd(&strm);
    return 1;
}
