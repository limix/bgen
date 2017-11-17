#ifndef BGEN_BGEN_H
#define BGEN_BGEN_H

#include <stdlib.h>
#include "bgen/number.h"
#include "bgen/string.h"

#define FAIL EXIT_FAILURE
#define SUCCESS EXIT_SUCCESS

typedef struct BGenFile BGenFile;

typedef struct Variant {
    string id;
    string rsid;
    string chrom;
    inti position;
    inti nalleles;
    string *allele_ids;
} Variant;

typedef struct VariantGenotype VariantGenotype;

typedef struct VariantIndexing VariantIndexing;

// Usual call flow:
// bgen_read
//     bgen_read_samples
//         ...
//     bgen_free_samples
//     bgen_read_variants / bgen_load_variants
//         bgen_free_variants (once)
//         bgen_open_variant_genotype
//             ...
//         bgen_close_variant_genotype
//     bgen_free_indexing
// bgen_close

// Any interaction should happen between open and close calls.
BGenFile *bgen_open(const byte *);
void bgen_close(BGenFile *);

// Variant headers can be stored in a file for faster reloading.
inti bgen_store_variants(const BGenFile *, Variant *, const byte *);
Variant *bgen_load_variants(const BGenFile *, const byte *, VariantIndexing **);

inti bgen_nsamples(BGenFile *bgen);
inti bgen_nvariants(BGenFile *bgen);

// Gets array of sample identifications ought to be freed by calling
// bgen_free_samples.
string *bgen_read_samples(BGenFile *bgen);
void bgen_free_samples(const BGenFile *bgen, string *samples);

// Variant headers are read all at once via either bgen_read_variants
// or bgen_load_variants. The second option exists because the first one
// might be too slow for some IOs as it requires jumping between blocks
// covering the entire files.
// Additional info: those blocks are in total very small compared to usual bgen
// file sizes, and are always in increasing order of serial positioning.
// However, a high number of variants will require a high number of access
// requests, which could be slow even for random access IOs (a network file
// system, e.g.).
Variant *bgen_read_variants(BGenFile *bgen, VariantIndexing **index);
void bgen_free_variants(const BGenFile *bgen, Variant *variants);
void bgen_free_indexing(VariantIndexing *index);

// Genotype interaction should happen between open and close call using the
// following functions; and those open and close calls should happen between
// read and close calls of bgen_read_variants/bgen_load_variants and
// bgen_free_indexing.
VariantGenotype *bgen_open_variant_genotype(VariantIndexing *, inti);
void bgen_close_variant_genotype(VariantIndexing *index, VariantGenotype *vg);

// Retrieve properties of a genotype specified by its variant.
void bgen_read_variant_genotype(VariantIndexing *, VariantGenotype *, real *);
inti bgen_nalleles(VariantGenotype *vg);
inti bgen_ploidy(VariantGenotype *vg);
inti bgen_ncombs(VariantGenotype *vg);

inti bgen_sample_ids_presence(BGenFile *bgen);

#endif /* end of include guard: BGEN_BGEN_H */
