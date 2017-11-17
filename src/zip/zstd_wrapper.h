#ifndef BGEN_ZSTD_WRAPPER_H
#define BGEN_ZSTD_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

#include "bgen/number.h"

inti bgen_zstd_uncompress(const byte *, inti, byte **, inti *);

#endif /* end of include guard: BGEN_ZSTD_WRAPPER_H */
