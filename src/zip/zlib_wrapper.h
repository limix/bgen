#ifndef BGEN_ZLIB_WRAPPER_H
#define BGEN_ZLIB_WRAPPER_H

#include <stddef.h>

int bgen_unzlib(const char *, size_t, char **, size_t *);
int bgen_unzlib_chunked(const char *, size_t, char **, size_t *);

#endif /* end of include guard: BGEN_ZLIB_WRAPPER_H */
