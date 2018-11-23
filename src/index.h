#ifndef INDEX3_H
#define INDEX3_H

#include "bgen.h"
#include <stdio.h>
#include <stdlib.h>

struct cmf;

struct cmf *create_metafile(const char *, uint32_t, uint32_t);
int write_metafile(struct cmf *, struct bgen_var *(*next)(uint64_t *, void *), void *,
                   int);
int close_metafile(struct cmf *);

#endif
