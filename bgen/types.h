#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

typedef unsigned char byte;
typedef int_fast64_t  inti;
typedef double        real;

typedef struct string {
    inti  len;
    byte *str;
} string;

static inline void string_alloc(string *v, inti len)
{
    v->len = len;
    v->str = malloc(len);
}

#endif /* end of include guard: TYPES_H */
