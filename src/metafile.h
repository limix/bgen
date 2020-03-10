#ifndef BGEN_METAFILE_H_PRIVATE
#define BGEN_METAFILE_H_PRIVATE

#include <inttypes.h>
#include <stdio.h>

/* variant metadata index */
struct bgen_idx
{
    uint32_t nvariants;
    /* size of the variants metadata block */
    uint64_t metasize;
    uint32_t npartitions;
    /* array of partition offsets */
    uint64_t* poffset;
};

/* node for creating metadata file */
struct bgen_mf
{
    char*           filepath;
    FILE*           stream;
    struct bgen_idx idx;
};

#endif
