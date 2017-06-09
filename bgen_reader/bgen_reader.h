#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdint.h>
#include <stdio.h>
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
    uint32_t  length;
    uint32_t  nsamples;
    SampleId *sampleids;
} SampleIdBlock;

typedef struct
{
    uint32_t length;
    BYTE    *id;
} Allele;

typedef struct
{
    uint32_t *ui_probs;
} SampleProbs;

typedef struct
{
    uint32_t     nsamples;
    uint16_t     nalleles;
    uint8_t      min_ploidy;
    uint8_t      max_ploidy;
    uint8_t     *missingness;
    uint8_t      phased;
    uint8_t      nbits;
    SampleProbs *sample_probs;
} VariantProbsBlock;

typedef struct
{
    uint32_t           nsamples;
    uint16_t           id_length;
    BYTE              *id;
    uint16_t           rsid_length;
    BYTE              *rsid;
    uint16_t           chrom_length;
    BYTE              *chrom;
    uint32_t           position;
    uint16_t           nalleles;
    Allele            *alleles;
    long               genotype_start;
    VariantProbsBlock *vpb;
} VariantBlock;

typedef struct
{
    char          *filepath;
    FILE *restrict file;
    Header         header;
    SampleIdBlock  sampleid_block;
    long           variants_start;
} BGenFile;

int64_t bgen_reader_open(BGenFile *,
                         char *);
int64_t bgen_reader_close(BGenFile *);

int64_t bgen_reader_layout(BGenFile *);
int64_t bgen_reader_compression(BGenFile *);
int64_t bgen_reader_sampleids(BGenFile *);
int64_t bgen_reader_nsamples(BGenFile *);
int64_t bgen_reader_nvariants(BGenFile *);

// Returns the sample identification
int64_t bgen_reader_sampleid(BGenFile *bgenfile,
                             uint64_t  sample_idx,
                             BYTE    **id,
                             uint64_t *length);

// Returns the variant identification
int64_t bgen_reader_variantid(BGenFile *bgenfile,
                              uint64_t  variant_idx,
                              BYTE    **id,
                              uint64_t *length);

// Returns the variant rsid
int64_t bgen_reader_variant_rsid(BGenFile *bgenfile,
                                 uint64_t  variant_idx,
                                 BYTE    **rsid,
                                 uint64_t *length);

// Returns identification of the variant's chromossome
int64_t bgen_reader_variant_chrom(BGenFile *bgenfile,
                                  uint64_t  variant_idx,
                                  BYTE    **chrom,
                                  uint64_t *length);

// Returns variant's position
int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t *position);

// Returns the number of alleles a variant has
int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t *nalleles);

int64_t bgen_reader_variant_ncombs(BGenFile *bgenfile,
                                   uint64_t  variant_idx,
                                   uint64_t *ncombs);

// Returns the allele identification of a variant
int64_t bgen_reader_variant_alleleid(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t  allele_idx,
                                     BYTE    **id,
                                     uint64_t *length);

int64_t bgen_reader_genotype_block(BGenFile     *bgenfile,
                                   uint64_t      variant_idx,
                                   VariantBlock *vb,
                                   uint32_t     *ui_probs);

#endif /* end of include guard: BGEN_READER_H */
