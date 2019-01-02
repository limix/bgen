#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Buffer;

struct Buffer *buffer_create();
int buffer_append(struct Buffer *b, void *mem, size_t size);
void *buffer_base(struct Buffer *b);
int buffer_destroy(struct Buffer *b);
int buffer_store(const char *fp, struct Buffer *b);
int buffer_load(const char *fp, struct Buffer *b, int verbose);

#endif /* BUFFER_H */
