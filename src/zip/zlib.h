#ifndef BGEN_ZIP_ZLIB_H
#define BGEN_ZIP_ZLIB_H

#include <stddef.h>

int bgen_unzlib(char const* src, size_t src_size, char** dst, size_t* dst_size);
int bgen_unzlib_chunked(const char*, size_t, char**, size_t*);

#endif
