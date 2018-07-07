#ifndef BGEN_UTIL_BUFFER_H
#define BGEN_UTIL_BUFFER_H

#include <stdlib.h>
#include <string.h>
#include "util/stdint.h"

struct Buffer;

struct Buffer *buffer_create();
int buffer_append(struct Buffer *b, void *mem, size_t size);
void *buffer_base(struct Buffer *b);
int buffer_destroy(struct Buffer *b);
int buffer_store(const char *fp, struct Buffer *b);
int buffer_load(const char *fp, struct Buffer *b, int verbose);


#endif /* end of include guard: BGEN_UTIL_BUFFER_H */
