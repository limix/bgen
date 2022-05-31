#ifndef BGEN_UNZSTD_H
#define BGEN_UNZSTD_H

#include <stddef.h>

int bgen_unzstd(char const* src, size_t src_size, void** dst, size_t* dst_size);

#endif
