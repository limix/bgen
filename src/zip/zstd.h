#ifndef BGEN_ZIP_ZSTD_H
#define BGEN_ZIP_ZSTD_H

#include <stddef.h>

int bgen_unzstd(char const* src, size_t src_size, void** dst, size_t* dst_size);

#endif
