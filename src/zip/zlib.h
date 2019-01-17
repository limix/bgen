#ifndef _BGEN_ZLIB_H
#define _BGEN_ZLIB_H

#include <assert.h>
#include <stddef.h>
#include <zlib.h>

#ifndef static_assert
#define static_assert _Static_assert
#endif
static_assert(sizeof(size_t) >= sizeof(uInt), "Safe conversion for zlib.");

int bgen_unzlib(const char *, size_t, char **, size_t *);
int bgen_unzlib_chunked(const char *, size_t, char **, size_t *);

#endif /* _BGEN_ZLIB_H */
