#ifndef _BGEN_ZSTD_WRAPPER_H
#define _BGEN_ZSTD_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

int bgen_unzstd(const char *, size_t, void **, size_t *);
char *bgen_zstd(const char *src, size_t src_size, size_t *dst_size);

#endif /* _BGEN_ZSTD_WRAPPER_H */
