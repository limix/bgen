#ifndef _BGEN_ZLIB_H
#define _BGEN_ZLIB_H

#include <stddef.h>
#include <zlib.h>

/* static_assert(sizeof(size_t) >= sizeof(uInt), "Safe conversion for zlib."); */

int bgen_unzlib(const char *, size_t, char **, size_t *);
int bgen_unzlib_chunked(const char *, size_t, char **, size_t *);

#endif /* _BGEN_ZLIB_H */
