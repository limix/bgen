#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdio.h>
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
    byte          *filepath;
    FILE *restrict file;
    inti           compression;
    inti           layout;
    inti          *start;
} VariantIndexing;

typedef struct BGenFile
{
    byte          *filepath;
    FILE *restrict file;
    inti           nvariants;
    inti           nsamples;
    inti           compression;
    inti           layout;
    inti           sample_ids_presence;
    inti           samples_start;
    inti           variants_start;
} BGenFile;

BGenFile       * bgen_open(const byte *filepath);
void             bgen_close(BGenFile *bgen);

inti             bgen_nsamples(BGenFile *bgen);
inti             bgen_nvariants(BGenFile *bgen);

string         * bgen_read_samples(BGenFile *bgen);
void             bgen_free_samples(const BGenFile *bgen,
                                   string         *samples);
Variant        * bgen_read_variants(BGenFile         *bgen,
                                    VariantIndexing **index);
void             bgen_free_variants(const BGenFile *bgen,
                                    Variant        *variants);
void             bgen_free_indexing(VariantIndexing *index);

VariantGenotype* bgen_read_variant_genotypes(VariantIndexing *indexing,
                                             inti             variant_start,
                                             inti             variant_end);

void bgen_free_variant_genotypes(VariantGenotype *vg,
                                 inti             nvariants);

#endif /* end of include guard: BGEN_READER_H */
