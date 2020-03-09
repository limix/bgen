#ifndef _BGEN_INDEX_H
#define _BGEN_INDEX_H

#include "file.h"
#include <stdint.h>

struct bgen_vi
{
    char *filepath;
    uint32_t compression;
    uint32_t layout;
    uint32_t nsamples;
    uint32_t nvariants;
    uint32_t max_nalleles;
    uint64_t *start;
};

int init_index(struct bgen_file const *, struct bgen_vi *);

#endif /* _BGEN_INDEX_H */
