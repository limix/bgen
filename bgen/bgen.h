#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdlib.h>

#include "types.h"

typedef struct Variant
{
    string  id;
    string  rsid;
    string  chrom;
    inti    position;
    inti    nalleles;
    string *allele_ids;
} Variant;

typedef struct VariantGenotype
{
    inti  ploidy;
    real *probabilities;
} VariantGenotype;

typedef struct VariantIndexing
{
    byte *filepath;
    inti  compression;
    inti  layout;
    inti *start;
} VariantIndexing;

typedef struct BGenFile
{
    byte          *filepath;
    FILE *restrict file;
    inti           nvariants;
    inti           nsamples;

    // 0: no compression
    // 1: zlib's compress()
    // 2: zstandard's ZSTD_compress()
    inti compression;

    // Possible values are 1 and 2.
    inti layout;
    inti sample_ids_presence;
    inti samples_start;
    inti variants_start;
} BGenFile;

BGenFile* bgen_open(const byte *filepath);
void      bgen_close(BGenFile *bgen);

inti      bgen_nsamples(BGenFile *bgen);
inti      bgen_nvariants(BGenFile *bgen);

string  * bgen_read_samples(BGenFile *bgen);
void      bgen_free_samples(const BGenFile *bgen,
                            string         *samples);
Variant * bgen_read_variants(BGenFile         *bgen,
                             VariantIndexing **index);

void      bgen_free_indexing(VariantIndexing *index);

// VariantGenotype* bgen_read_variant_genotypes(VariantIndexing *indexing,
//                                              inti             variant_start,
//                                              inti             variant_end);

#endif /* end of include guard: BGEN_READER_H */
