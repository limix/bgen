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
        printf("zlib failed to uncompress: %s.\n", zError(e));
        return EXIT_FAILURE;
    }


    strm.avail_in = src_size;

    strm.avail_out = *dst_size;
    strm.next_out  = *dst;

    e = inflate(&strm, Z_NO_FLUSH);
    assert(e != Z_STREAM_ERROR); /* state not clobbered */

    switch (e) {
    case Z_NEED_DICT:
        e = Z_DATA_ERROR; /* and fall through */

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

#endif /* end of include guard: ZLIB_WRAPPER_H */