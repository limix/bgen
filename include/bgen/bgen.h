#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

typedef unsigned char byte;
typedef int_fast64_t  inti;
typedef double        real;

typedef struct string {
    inti  len;
    byte *str;
} string;

typedef struct BGenFile BGenFile;

typedef struct Variant
{
    string  id;
    string  rsid;
    string  chrom;
    inti    position;
    inti    nalleles;
    string *allele_ids;
} Variant;

typedef struct VariantGenotype VariantGenotype;

typedef struct VariantIndexing VariantIndexing;

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

VariantGenotype* bgen_open_variant_genotype(VariantIndexing *indexing,
                                            inti             variant_idx);

void             bgen_read_variant_genotype(VariantIndexing *indexing,
                                            VariantGenotype *vg,
                                            real            *probabilities,
                                            inti            *missingness);

inti bgen_variant_genotype_nsamples(VariantGenotype *vg);
inti bgen_variant_genotype_nalleles(VariantGenotype *vg);
inti bgen_variant_genotype_ploidy(VariantGenotype *vg);
inti bgen_variant_genotype_ncombs(VariantGenotype *vg);

void bgen_close_variant_genotype(VariantIndexing *indexing,
                                 VariantGenotype *vg);

inti bgen_sample_ids_presence(BGenFile *bgen);

#endif /* end of include guard: BGEN_READER_H */
