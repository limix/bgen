#ifndef BGEN_UTIL_BUFFER_H
#define BGEN_UTIL_BUFFER_H

#include <stdlib.h>
#include <string.h>

#include "bgen/number.h"

struct Buffer;

struct Buffer *buffer_create();
inti buffer_append(struct Buffer *b, void *mem, size_t size);
void *buffer_base(struct Buffer *b);
size_t buffer_size(struct Buffer *b);
inti buffer_destroy(struct Buffer *b);
inti buffer_store(const char *fp, struct Buffer *b);
inti buffer_load(const char *fp, struct Buffer *b);


#endif /* end of include guard: BGEN_UTIL_BUFFER_H */
