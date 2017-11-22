#include "mem.h"

char *bgen_strdup(const char *src) {
    char *str;
    char *p;
    int len = 0;

    while (src[len])
        len++;
    str = (char *)malloc(len + 1);
    p = str;

    while (*src)
        *p++ = *src++;
    *p = '\0';
    return str;
}

void bgen_memcpy(void *dst, char **src, size_t n) {
    memcpy(dst, *src, n);
    *src = *src + n;
}
