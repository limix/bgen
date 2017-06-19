#include "mem.h"

byte* bgen_reader_strndup(const byte *src, inti size)
{
    byte *str = malloc(size);

    memcpy(str, src, size);
    return str;
}
