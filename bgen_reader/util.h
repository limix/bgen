#ifndef UTIL_H
#define UTIL_H

#include <zlib.h>
#include <stdlib.h>
#include <string.h>

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
                                      BYTE **dst, uint32_t *dst_size)
{
    uLongf z_dst_size = *dst_size;

    int err = uncompress(*dst, &z_dst_size, src, src_size);

    if (err != Z_OK)
    {
        printf("zlib failed to uncompress: %s.\n", zError(err));
        return EXIT_FAILURE;
    }

    *dst_size = z_dst_size;

    *dst = realloc(*dst, *dst_size);

    return EXIT_SUCCESS;
}

#endif /* end of include guard: UTIL_H */

inline static void MEMCPY(void *dst, BYTE **src, size_t n)
{
        memcpy(dst, *src, n);
        *src = *src + n;
}
