#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdint.h>

#include "types.h"
#include "user_structs.h"
#include "impl_structs.h"
#include "variant_id_block.h"

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
    char *filename;
    inti  compression;
    inti  layout;
    inti *start;
} VariantIndexing;

BGenFile       * bgen_open(const char *filepath);
inti             bgen_close(BGenFile *bgen);

inti             bgen_nsamples(BGenFile *bgen);
inti             bgen_nvariants(BGenFile *bgen);

string         * bgen_read_samples(BGenFile *bgen);
Variant        * bgen_read_variants(BGenFile *bgen);

VariantIndexing* bgen_create_variant_index(BGenFile *bgen);

VariantGenotype* bgen_read_variant_genotypes(VariantIndexing *indexing,
                                             inti             variant_start,
                                             inti             variant_end);

#endif /* end of include guard: BGEN_READER_H */
