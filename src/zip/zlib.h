#ifndef BGEN_ZIP_ZLIB_H
#define BGEN_ZIP_ZLIB_H

#include <stddef.h>

int bgen_unzlib(const char*, size_t, char**, size_t*);
int bgen_unzlib_chunked(const char*, size_t, char**, size_t*);

#endif
