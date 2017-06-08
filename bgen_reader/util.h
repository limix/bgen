#ifndef UTIL_H
#define UTIL_H

#include <zlib.h>
#include <stdlib.h>
#include <string.h>

// Duplicate a string.
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

inline static void MEMCPY(void *dst, BYTE **src, size_t n)
{
    memcpy(dst, *src, n);
    *src = *src + n;
}

inline static uint64_t choose(uint64_t n, uint64_t k)
{
    uint64_t ans = 1;

    k = k > n - k ? n - k : k;
    int j = 1;

    for (; j <= k; j++, n--)
    {
        if (n % j == 0)
        {
            ans *= n / j;
        } else
        if (ans % j == 0)
        {
            ans = ans / j * n;
        } else
        {
            ans = (ans * n) / j;
        }
    }
    return ans;
}

inline static size_t bit_to_byte_idx(size_t bi)
{
    return bi / 8;
}

inline static size_t bit_in_byte_idx(size_t bi)
{
    return bi % 8;
}

#endif /* end of include guard: UTIL_H */
