#ifndef UTIL_H
#define UTIL_H

#include <zlib.h>
#include <stdlib.h>

inline static char* ft_strdup(char *src)
{
    char *str;
    char *p;
    int   len = 0;

    while (src[len]) len++;
    str = malloc(len + 1);
    p   = str;

    while (*src) *p++ = *src++;
    *p = '\0';
    return str;
}

inline static int64_t zlib_uncompress(const BYTE *src, size_t src_size,
                                      BYTE **dst, size_t *dst_size)
{
    uLongf z_dst_size = *dst_size;

    int result = uncompress(*dst, &z_dst_size, src, src_size);

    *dst_size = z_dst_size;

    *dst = realloc(*dst, *dst_size);

    // assert( result == Z_OK ) ;
    // assert( dest_size % sizeof( T ) == 0 ) ;
    // dest->resize( dest_size / sizeof( T )) ;
    return 0;
}

#endif /* end of include guard: UTIL_H */
