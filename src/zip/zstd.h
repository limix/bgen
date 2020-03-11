#ifndef BGEN_ZIP_ZSTD_H
#define BGEN_ZIP_ZSTD_H

#include <stddef.h>

int   bgen_unzstd(const char*, size_t, void**, size_t*);
char* bgen_zstd(const char* src, size_t src_size, size_t* dst_size);

#endif
