#include "mem.h"

byte *bgen_strdup(const byte *src) {
    byte *str;
    byte *p;
    int len = 0;

    while (src[len])
        len++;
    str = (byte *)malloc(len + 1);
    p = str;

    while (*src)
        *p++ = *src++;
    *p = '\0';
    return str;
}

void bgen_memcpy(void *dst, byte **src, inti n) {
    memcpy(dst, *src, n);
    *src = *src + n;
}
