#ifndef SAMPLE_H
#define SAMPLE_H

#include <stdint.h>

#include "types.h"

typedef struct
{
    inti  length;
    byte *id;
} SampleId;

typedef struct SampleIdBlock
{
    inti      length;
    inti      nsamples;
    SampleId *sampleids;
} SampleIdBlock;

typedef struct BGenFile BGenFile;

inti bgen_read_sampleid_block(BGenFile *bgenfile);

#endif /* end of include guard: SAMPLE_H */
