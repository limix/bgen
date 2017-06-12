#ifndef SAMPLE_H
#define SAMPLE_H

#include <stdint.h>

#include "util/byte.h"

typedef struct
{
    uint16_t length;
    BYTE    *id;
} SampleId;

typedef struct SampleIdBlock
{
    uint32_t  length;
    uint32_t  nsamples;
    SampleId *sampleids;
} SampleIdBlock;

typedef struct BGenFile BGenFile;

int64_t bgen_read_sampleid_block(BGenFile *bgenfile);

#endif /* end of include guard: SAMPLE_H */