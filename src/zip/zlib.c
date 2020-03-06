#include "zip/zlib.h"
#include "bgen/bgen.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

int bgen_unzlib(const char *src, size_t src_size, char **dst, size_t *dst_size)
{
    int e;
    z_stream strm;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = (unsigned char *)src;
    e = inflateInit(&strm);

    if (e != Z_OK) {
        fprintf(stderr, "zlib failed to uncompress: %s.\n", zError(e));
        return EXIT_FAILURE;
    }

    strm.avail_in = (uInt)src_size;

    strm.avail_out = (uInt)*dst_size;
    strm.next_out = (unsigned char *)*dst;

    e = inflate(&strm, Z_NO_FLUSH);
    if (e == Z_NEED_DICT) {
        e = Z_DATA_ERROR;
        goto err;
    }

    if (e == Z_DATA_ERROR || e == Z_MEM_ERROR)
        goto err;

    inflateEnd(&strm);
    return EXIT_SUCCESS;
err:
    inflateEnd(&strm);
    fprintf(stderr, "zlib failed to uncompress: %s.\n", zError(e));
    return EXIT_FAILURE;
}

int bgen_unzlib_chunked(const char *src, size_t src_size, char **dst, size_t *dst_size)
{
    int ret;
    z_stream strm;

    uInt unused = *dst_size;
    char *cdst = *dst;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);

    if (ret != Z_OK) {
        fprintf(stderr, "zlib failed to uncompress: %s.\n", zError(ret));
        return EXIT_FAILURE;
    }

    strm.avail_in = (uInt)src_size;
    strm.next_in = (unsigned char *)src;

    while (1) {
        strm.avail_out = unused;
        strm.next_out = (unsigned char *)cdst;

        ret = inflate(&strm, Z_NO_FLUSH);

        if (ret == Z_NEED_DICT) {
            ret = Z_DATA_ERROR;
            goto err;
        }

        if (ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
            goto err;

        uInt just_wrote = unused - strm.avail_out;

        cdst += just_wrote;
        unused -= just_wrote;

        if (ret == Z_STREAM_END) {
            *dst_size -= unused;
            *dst = (char *)realloc(*dst, *dst_size);
            break;
        }

        if (strm.avail_out == 0) {
            if (unused > 0) {
                fprintf(stderr, "zlib failed to uncompress: unknown error.\n");
                return EXIT_FAILURE;
            }

            unused = (uInt)*dst_size;
            *dst_size += unused;
            *dst = (char *)realloc(*dst, *dst_size);
            cdst = *dst + unused;
        }
    }

    inflateEnd(&strm);
    return EXIT_SUCCESS;
err:
    inflateEnd(&strm);
    fprintf(stderr, "zlib failed to uncompress: %s.\n", zError(ret));
    return EXIT_FAILURE;
}
