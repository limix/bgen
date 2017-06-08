#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdint.h>
#include "byte.h"

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
        BYTE    *id;
} SampleId;

typedef struct
{
        uint32_t length;
        uint32_t nsamples;
        SampleId *sampleids;
} SampleIdBlock;

typedef struct
{
        uint32_t length;
        BYTE    *id;
} Allele;

typedef struct
{
        uint32_t nsamples;
        uint16_t id_length;
        BYTE    *id;
        uint16_t rsid_length;
        BYTE    *rsid;
        uint16_t chrom_length;
        BYTE    *chrom;
        uint32_t position;
        uint16_t nalleles;
        Allele  *alleles;
        long genotype_start;
} VariantBlock;

typedef struct
{
        char         *filepath;
        Header header;
        SampleIdBlock sampleid_block;
        long variants_start;
} BGenFile;

int64_t bgen_reader_read(BGenFile *, char *);
int64_t bgen_reader_layout(BGenFile *);
int64_t bgen_reader_compression(BGenFile *);
int64_t bgen_reader_sample_identifiers(BGenFile *);
int64_t bgen_reader_nsamples(BGenFile *);
int64_t bgen_reader_nvariants(BGenFile *);
int64_t bgen_reader_sample_id(BGenFile *, uint64_t, BYTE **, uint64_t *);
int64_t bgen_reader_variant_id(BGenFile *, uint64_t, BYTE **, uint64_t *);
int64_t bgen_reader_variant_rsid(BGenFile *, uint64_t, BYTE **, uint64_t *);
int64_t bgen_reader_variant_chrom(BGenFile *, uint64_t, BYTE **, uint64_t *);
int64_t bgen_reader_variant_position(BGenFile *, uint64_t);
int64_t bgen_reader_variant_nalleles(BGenFile *, uint64_t);
int64_t bgen_reader_variant_allele_id(BGenFile *, uint64_t, uint64_t, BYTE **,
                                      uint64_t *);
int64_t bgen_reader_genotype_block(BGenFile *, uint64_t, VariantBlock *);

#endif /* ifndef BGEN_READER_H */
