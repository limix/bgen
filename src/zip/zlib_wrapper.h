#ifndef BGEN_ZLIB_WRAPPER_H
#define BGEN_ZLIB_WRAPPER_H

#include "bgen/number.h"

inti bgen_unzlib(const byte *, inti, byte **, inti *);
inti bgen_unzlib_chunked(const byte *, inti, byte **, inti *);

#endif /* end of include guard: BGEN_ZLIB_WRAPPER_H */
