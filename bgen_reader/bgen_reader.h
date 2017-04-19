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

// typedef struct
// {
//     uint32_t length;
//     uint16_t *prob_chunk;
// } Probability;

typedef struct
{
    uint32_t length;
    char *id;
} Allele;

typedef struct
{
    uint32_t  nsamples;
    uint16_t  id_length;
    char    *id;
    uint16_t  rsid_length;
    char    *rsid;
    uint16_t  chrom_length;
    char    *chrom;
    uint32_t position;
    uint16_t  nalleles;
    Allele *alleles;
} VariantBlock;

typedef struct
{
    char *filepath;
    Header        header;
    SampleIdBlock sampleid_block;
    long variants_start;
} BGenFile;

int64_t bgen_reader_read(BGenFile *, char *);
int64_t bgen_reader_layout(BGenFile *);
int64_t bgen_reader_compression(BGenFile *);
int64_t bgen_reader_sample_identifiers(BGenFile *);
int64_t bgen_reader_nsamples(BGenFile *);
int64_t bgen_reader_nvariants(BGenFile *);
int64_t bgen_reader_sample_id(BGenFile *, uint64_t, char **, uint64_t *);
int64_t bgen_reader_variant_id(BGenFile *, uint64_t, char **, uint64_t *);
int64_t bgen_reader_variant_rsid(BGenFile *, uint64_t, char **, uint64_t *);
int64_t bgen_reader_variant_chrom(BGenFile *, uint64_t, char **, uint64_t *);

#endif /* ifndef BGEN_READER_H */
