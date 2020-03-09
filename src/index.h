#ifndef BGEN_INDEX_H
#define BGEN_INDEX_H

#include "file.h"
#include <stdint.h>

struct bgen_vi
{
    char const* filepath;
    uint32_t    compression;
    uint32_t    layout;
    uint32_t    nsamples;
    uint32_t    nvariants;
    uint32_t    max_nalleles;
};

int init_index(struct bgen_file const*, struct bgen_vi*);

static inline struct bgen_vi BGEN_VI(struct bgen_file const* bgen)
{
    return (struct bgen_vi){
        bgen_file_filepath(bgen),
        (uint32_t)bgen_file_compression(bgen),
        (uint32_t)bgen_file_layout(bgen),
        (uint32_t)bgen_file_nsamples(bgen),
        (uint32_t)bgen_file_nvariants(bgen),
        0,
    };
}

#endif
