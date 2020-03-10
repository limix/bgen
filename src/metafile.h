#ifndef BGEN_METAFILE_H_PRIVATE
#define BGEN_METAFILE_H_PRIVATE

#include <inttypes.h>
#include <stdio.h>

struct bgen_mf
{
    char*    filepath;
    FILE*    stream;
    uint32_t nvariants;
    uint64_t metasize; /**< Size of the metadata block */
    uint32_t npartitions;
    uint64_t* poffset; /**< Array of partition offsets */
};

#endif
