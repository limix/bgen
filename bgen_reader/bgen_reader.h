#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdint.h>

typedef struct
{
    uint32_t offset;
    uint32_t header_length;
    uint32_t nvariants;
    uint32_t nsamples;
    uint32_t magic_number;
    uint32_t flags;
} Header;

typedef struct
{
    uint16_t length;
    char    *id;
} SampleId;

typedef struct
{
    uint32_t  length;
    uint32_t  nsamples;
    SampleId *sampleids;
} SampleIdBlock;

typedef struct
{
    Header        header;
    SampleIdBlock sampleid_block;
} BGenFile;

int64_t bgen_reader_read(BGenFile *);
int64_t bgen_reader_nsamples(BGenFile *);
int64_t bgen_reader_nvariants(BGenFile *);
int64_t bgen_reader_sample_id(BGenFile *, uint64_t, char **, uint64_t *);

#endif /* ifndef BGEN_READER_H */
