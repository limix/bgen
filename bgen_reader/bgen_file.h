#ifndef BGEN_FILE_H
#define BGEN_FILE_H

#include <stdint.h>
#include <stdio.h>

typedef struct SampleIdBlock SampleIdBlock;

// flags definition:
// bit | name                | value | description
// 0-1 | CompressedSNPBlocks | 0     | SNP block probability data is not
//                                     compressed
// 0-1 | CompressedSNPBlocks | 1     | SNP block probability data is compressed
//                                     using zlib's compressed()
// 0-1 | CompressedSNPBlocks | 2     | SNP block probability data is compressed
//                                     using zstandard's ZSTD_compress()
// 2-5 | Layout              | 0     | this value is not supported
// 2-5 | Layout              | 1     | layout 1
// 2-5 | Layout              | 2     | layout 2
// 31  | SampleIdentifiers   | 0     | sample identifiers are not stored
// 31  | SampleIdentifiers   | 1     | sample identifier block follows header
typedef struct Header
{
    uint32_t offset;
    uint32_t header_length;
    uint32_t nvariants;
    uint32_t nsamples;
    uint32_t magic_number;
    uint32_t flags;
} Header;

typedef struct BGenFile
{
    char          *filepath;
    FILE *restrict file;
    Header         header;
    SampleIdBlock *sampleid_block;
    long           variants_start;
} BGenFile;

int64_t bgen_reader_read_header(BGenFile *bgenfile, Header *header);

int64_t bgen_reader_fopen(BGenFile *bgenfile);
int64_t bgen_reader_fclose(BGenFile *bgenfile);

int64_t bgen_reader_fread(BGenFile      *bgenfile,
                          void *restrict buffer,
                          size_t         size);

#endif /* end of include guard: BGEN_FILE_H */
