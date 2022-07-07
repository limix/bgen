#ifndef BGEN_UNZLIB_H
#define BGEN_UNZLIB_H

#include <stddef.h>

int bgen_unzlib(char const* src, size_t src_size, char** dst, size_t* dst_size);
int bgen_unzlib_chunked(char const* src, size_t src_size, char** dst, size_t* dst_size);

#endif
