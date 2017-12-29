#ifndef BGEN_BGEN_H
#define BGEN_BGEN_H

#include "bgen/string.h"
#include <stdlib.h>

#ifdef WIN32
#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *)__nan)
#endif
#endif

struct BGenFile;
struct BGenVI;

struct BGenVar {
    string id;
    string rsid;
    string chrom;
    int position;
    int nalleles;
    string *allele_ids;
};

struct BGenVG;

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
//     bgen_free_index
// bgen_close

// Any interaction should happen between open and close calls.
struct BGenFile *bgen_open(const char *);
void bgen_close(struct BGenFile *);

// BGenVar headers can be stored in a file for faster reloading.
int bgen_store_variants(const struct BGenFile *, struct BGenVar *,
                        struct BGenVI *, const char *);
struct BGenVar *bgen_load_variants(const struct BGenFile *, const char *,
                                   struct BGenVI **, int);
int bgen_create_variants_index_file(const char *bgen_fp, const char *index_fp);

int bgen_nsamples(struct BGenFile *bgen);
int bgen_nvariants(struct BGenFile *bgen);

// Gets array of sample identifications ought to be freed by calling
// bgen_free_samples.
string *bgen_read_samples(struct BGenFile *bgen);
void bgen_free_samples(const struct BGenFile *bgen, string *samples);

// BGenVar headers are read all at once via either bgen_read_variants
// or bgen_load_variants. The second option exists because the first one
// might be too slow for some IOs as it requires jumping between blocks
// covering the entire files.
// Additional info: those blocks are in total very small compared to usual bgen
// file sizes, and are always in increasing order of serial positioning.
// However, a high number of variants will require a high number of access
// requests, which could be slow even for random access IOs (a network file
// system, e.g.).
struct BGenVar *bgen_read_variants(struct BGenFile *bgen,
                                   struct BGenVI **index);
void bgen_free_variants(const struct BGenFile *bgen, struct BGenVar *variants);
void bgen_free_index(struct BGenVI *index);

// Genotype interaction should happen between open and close call using the
// following functions; and those open and close calls should happen between
// read and close calls of bgen_read_variants/bgen_load_variants and
// bgen_free_index.
struct BGenVG *bgen_open_variant_genotype(struct BGenVI *, size_t);
void bgen_close_variant_genotype(struct BGenVI *index, struct BGenVG *vg);

// Retrieve properties of a genotype specified by its variant.
void bgen_read_variant_genotype(struct BGenVI *, struct BGenVG *, double *);
int bgen_nalleles(struct BGenVG *vg);
int bgen_ploidy(struct BGenVG *vg);
int bgen_ncombs(struct BGenVG *vg);

int bgen_sample_ids_presence(struct BGenFile *bgen);

#endif /* end of include guard: BGEN_BGEN_H */
