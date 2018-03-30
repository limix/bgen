#ifndef BGEN_BGEN_H
#define BGEN_BGEN_H

#include <math.h>
#include <stdlib.h>

typedef struct bgen_string {
  int len;
  char *str;
} bgen_string;

struct bgen_file; /* bgen file handler */
struct bgen_vi;   /* variant index */
struct bgen_vg;   /* variant genotype */

/* Variant metadata. */
struct bgen_var {
  bgen_string id;
  bgen_string rsid;
  bgen_string chrom;
  int position;
  int nalleles;
  bgen_string *allele_ids;
};

/* Open a file and return a bgen file handler. */
struct bgen_file *bgen_open(const char *filepath);
/* Close a bgen file handler. */
void bgen_close(struct bgen_file *bgen);

/* Get the number of samples. */
int bgen_nsamples(const struct bgen_file *bgen);
/* Get the number of variants. */
int bgen_nvariants(const struct bgen_file *bgen);
/* Check if the file contains sample identifications. */
int bgen_sample_ids_presence(const struct bgen_file *bgen);

/* Get array of sample identifications. */
bgen_string *bgen_read_samples(struct bgen_file *bgen, int verbose);
/* Free array of sample identifications. */
void bgen_free_samples(const struct bgen_file *bgen, bgen_string *samples);

/* Read variants metadata and generate variants index.

    Reading variants metadata (and generating the variants index) can be costly
    as it requires accessing chunks of data across the file. We therefore
    provide the functions
        - bgen_store_variants_metadata
        - bgen_load_variants_metadata
        - bgen_create_variants_metadata_file
    for storing and reading that information from an additional file. We refer
    to this file as variants metadata file.

    Note: remember to call `bgen_free_variants` and `bgen_free_index` after use.
*/
struct bgen_var *bgen_read_variants(struct bgen_file *bgen, struct bgen_vi **vi,
                                    int verbose);
void bgen_free_variants(const struct bgen_file *bgen,
                        struct bgen_var *variants);
void bgen_free_index(struct bgen_vi *vi);

/* Open a variant for genotype queries.

    A variant genotype handler is needed to call
        - bgen_read_variant_genotype
        - bgen_nalleles
        - bgen_ploidy
        - bgen_ncombs

    Note: remember to call `bgen_close_variant_genotype` after use.
 */
struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *vi, size_t index);
/* Close a variant genotype handler. */
void bgen_close_variant_genotype(struct bgen_vi *vi, struct bgen_vg *vg);

/* Read the probabilities of each possible genotype.

    The variant index and a variant genotype handler are required.
    The number of probabilities can be found from `bgen_ncombs`.
*/
void bgen_read_variant_genotype(struct bgen_vi *vi, struct bgen_vg *vg,
                                double *probs);
/* Get the number of alleles. */
int bgen_nalleles(const struct bgen_vg *vg);
/* Get the ploidy. */
int bgen_ploidy(const struct bgen_vg *vg);
/* Get the number of genotype combinations. */
int bgen_ncombs(const struct bgen_vg *vg);

/* Store variants metadata. */
int bgen_store_variants_metadata(const struct bgen_file *bgen,
                                 struct bgen_var *variants, struct bgen_vi *vi,
                                 const char *filepath);
/* Read variants metadata from file. */
struct bgen_var *bgen_load_variants_metadata(const struct bgen_file *bgen,
                                             const char *filepath,
                                             struct bgen_vi **vi, int verbose);
/* Create a variants metadata file.

    Helper for easy creation of variants metadata file.

    Note: this file is not part of the bgen file format specification.
*/
int bgen_create_variants_metadata_file(const char *bgen_fp, const char *vi_fp,
                                       int verbose);

#endif /* end of include guard: BGEN_BGEN_H */
