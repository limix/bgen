#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <string.h>

// Duplicate a string.
inline static char* ft_strdup(const char *src)
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

inline static void MEMCPY(void *dst, byte **src, size_t n)
{
    memcpy(dst, *src, n);
    *src = *src + n;
}

#endif /* end of include guard: MEM_H */
