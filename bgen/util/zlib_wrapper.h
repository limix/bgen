#ifndef ZLIB_WRAPPER_H
#define ZLIB_WRAPPER_H

#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>

#include "../types.h"

inline static inti zlib_uncompress(const byte *src, inti src_size,
                                   byte **dst, inti *dst_size)
{
    int e;
    z_stream strm;

    strm.zalloc   = Z_NULL;
    strm.zfree    = Z_NULL;
    strm.opaque   = Z_NULL;
    strm.avail_in = 0;
    strm.next_in  = (byte *)src;
    e             = inflateInit(&strm);

    if (e != Z_OK)
    {
        fprintf(stderr, "zlib failed to uncompress: %s.\n", zError(e));
        return EXIT_FAILURE;
    }


    strm.avail_in = src_size;

    strm.avail_out = *dst_size;
    strm.next_out  = *dst;

    e = inflate(&strm, Z_NO_FLUSH);
    assert(e != Z_STREAM_ERROR);

    switch (e) {
    case Z_NEED_DICT:
        e = Z_DATA_ERROR;

    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
        inflateEnd(&strm);
        printf("zlib failed to uncompress: %s.\n", zError(e));
        return EXIT_FAILURE;
    }

    assert(strm.avail_out == 0);

    inflateEnd(&strm);
    return EXIT_SUCCESS;
}

inline static inti zlib_uncompress_chunked(const byte *src, inti src_size,
                                           byte **dst, inti *dst_size)
{
    int ret;
    z_stream strm;

    unsigned int just_wrote;
    unsigned int unused = *dst_size;
    byte *cdst          = *dst;

    strm.zalloc   = Z_NULL;
    strm.zfree    = Z_NULL;
    strm.opaque   = Z_NULL;
    strm.avail_in = 0;
    strm.next_in  = Z_NULL;
    ret           = inflateInit(&strm);

    if (ret != Z_OK)
    {
        fprintf(stderr, "zlib failed to uncompress: %s.\n", zError(ret));
        return EXIT_FAILURE;
    }

    strm.avail_in = src_size;
    strm.next_in  = (byte *)src;

    while (1) {
        strm.avail_out = unused;
        strm.next_out  = cdst;

        ret = inflate(&strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);

        switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;

        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&strm);
            fprintf(stderr, "zlib failed to uncompress: %s.\n", zError(ret));
            return EXIT_FAILURE;
        }

        just_wrote = unused - strm.avail_out;

        cdst   += just_wrote;
        unused -= just_wrote;

        if (ret == Z_STREAM_END)
        {
            *dst_size -= unused;
            *dst       = realloc(*dst, *dst_size);
            break;
        }

        if (strm.avail_out == 0)
        {
            if (unused > 0)
            {
                fprintf(stderr, "zlib failed to uncompress: unknown error.\n");
                return EXIT_FAILURE;
            }

            unused     = *dst_size;
            *dst_size += unused;
            *dst       = realloc(*dst, *dst_size);
            cdst       = *dst + unused;
        }
    }

    inflateEnd(&strm);
    return EXIT_SUCCESS;
}

#endif /* end of include guard: ZLIB_WRAPPER_H */
