#ifndef BGEN_ZLIB_WRAPPER_H
#define BGEN_ZLIB_WRAPPER_H

#include "bgen/number.h"

inti bgen_zlib_uncompress(const byte *, inti, byte **, inti *);
inti bgen_zlib_uncompress_chunked(const byte *, inti, byte **, inti *);

#endif /* end of include guard: BGEN_ZLIB_WRAPPER_H */
