#ifndef BGEN_ZSTD_WRAPPER_H
#define BGEN_ZSTD_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

#include "bgen/number.h"

inti bgen_unzstd(const byte *, inti, byte **, inti *);
byte *bgen_zstd(const byte *src, inti src_size, inti *dst_size);

#endif /* end of include guard: BGEN_ZSTD_WRAPPER_H */
