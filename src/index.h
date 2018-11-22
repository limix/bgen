#ifndef INDEX3_H
#define INDEX3_H

#include "bgen.h"
#include <stdio.h>
#include <stdlib.h>

#define BGEN_IDX_NAME "bgen index "
#define BGEN_IDX_VER "03"
#define BGEN_HDR_LEN 13

struct cmf;

struct cmf *create_metafile(const char *, uint32_t, uint32_t);
int write_metafile(struct cmf *, struct bgen_var *(*next)(uint64_t *, void *), void *,
                   int);
int close_metafile(struct cmf *);

#endif
