#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "variant_genotype_fwd.h"
#include "variant_indexing_fwd.h"

typedef struct Variant {
    string id;
    string rsid;
    string chrom;
    inti position;
    inti nalleles;
    string *allele_ids;
} Variant;

typedef struct BGenFile {
    byte *filepath;
    FILE *file;
    inti nvariants;
    inti nsamples;
    inti compression;
    inti layout;
    inti sample_ids_presence;
    inti samples_start;
    inti variants_start;
} BGenFile;

BGenFile *bgen_open(const byte *filepath);
void bgen_close(BGenFile *bgen);

inti bgen_nsamples(BGenFile *bgen);
inti bgen_nvariants(BGenFile *bgen);

string *bgen_read_samples(BGenFile *bgen);
void bgen_free_samples(const BGenFile *bgen, string *samples);
Variant *bgen_read_variants(BGenFile *bgen, VariantIndexing **index);
void bgen_free_variants(const BGenFile *bgen, Variant *variants);
void bgen_free_indexing(VariantIndexing *index);

VariantGenotype *bgen_open_variant_genotype(VariantIndexing *index,
                                            inti variant_idx);

void bgen_read_variant_genotype(VariantIndexing *index, VariantGenotype *vg,
                                real *probabilities);

inti bgen_nalleles(VariantGenotype *vg);
inti bgen_ploidy(VariantGenotype *vg);
inti bgen_ncombs(VariantGenotype *vg);

void bgen_close_variant_genotype(VariantIndexing *index, VariantGenotype *vg);

inti bgen_sample_ids_presence(BGenFile *bgen);

#endif /* end of include guard: BGEN_READER_H */
