#ifndef BGEN_BGEN_H
#define BGEN_BGEN_H

#include <math.h>
#include <stdlib.h>

typedef struct bgen_string {
  int len;
  char *str;
} bgen_string;

#ifdef WIN32
#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *)__nan)
#endif
#endif

struct BGenFile;
struct BGenVI;

struct BGenVar {
  bgen_string id;
  bgen_string rsid;
  bgen_string chrom;
  int position;
  int nalleles;
  bgen_string *allele_ids;
};

struct BGenVG;

// Usual call flow:
// bgen_open
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
struct BGenFile *bgen_open(const char *filepath);
void bgen_close(struct BGenFile *bgen);

int bgen_nsamples(struct BGenFile *bgen);
int bgen_nvariants(struct BGenFile *bgen);
int bgen_sample_ids_presence(struct BGenFile *bgen);

// Get array of sample identifications
bgen_string *bgen_read_samples(struct BGenFile *bgen, int verbose);
// Free array of sample identifications
void bgen_free_samples(const struct BGenFile *bgen, bgen_string *samples);

// BGenVar headers are read all at once via either bgen_read_variants
// or bgen_load_variants. The second option exists because the first one
// might be too slow for some IOs as it requires jumping between blocks
// covering the entire files.
// Additional info: those blocks are in total very small compared to usual bgen
// file sizes, and are always in increasing order of serial positioning.
// However, a high number of variants will require a high number of access
// requests, which could be slow even for random access IOs (a network file
// system, e.g.).
struct BGenVar *bgen_read_variants(struct BGenFile *bgen, struct BGenVI **vi,
                                   int verbose);
void bgen_free_variants(const struct BGenFile *bgen, struct BGenVar *variants);
void bgen_free_index(struct BGenVI *vi);

// Genotype interaction should happen between open and close call using the
// following functions; and those open and close calls should happen between
// read and close calls of bgen_read_variants/bgen_load_variants and
// bgen_free_index.
struct BGenVG *bgen_open_variant_genotype(struct BGenVI *vi, size_t index);
void bgen_close_variant_genotype(struct BGenVI *vi, struct BGenVG *vg);

// Retrieve genotype properties of a variant.
void bgen_read_variant_genotype(struct BGenVI *vi, struct BGenVG *vg,
                                double *probs);
int bgen_nalleles(struct BGenVG *vg);
int bgen_ploidy(struct BGenVG *vg);
int bgen_ncombs(struct BGenVG *vg);

// Variants metadata file handling
int bgen_store_variants(const struct BGenFile *bgen, struct BGenVar *variants,
                        struct BGenVI *vi, const char *filepath);
struct BGenVar *bgen_load_variants(const struct BGenFile *bgen,
                                   const char *filepath, struct BGenVI **vi,
                                   int verbose);
int bgen_create_variants_index_file(const char *bgen_fp, const char *vi_fp,
                                    int verbose);

#endif /* end of include guard: BGEN_BGEN_H */
