/*******************************************************************************
 * Project: bgen
 * Purpose: BGEN file reader
 * Author: Danilo Horta, danilo.horta@gmail.com
 * Language: C
 *******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Danilo Horta
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#ifndef BGEN_BGEN_H
#define BGEN_BGEN_H

#define BGEN_VERSION "1.1.3"
#define BGEN_VERSION_MAJOR 1
#define BGEN_VERSION_MINOR 1
#define BGEN_VERSION_PATCH 3

#include <math.h>
#include <stdlib.h>

struct bgen_string {
  int len;
  char *str;
};

struct bgen_file; /* bgen file handler */
struct bgen_vi;   /* variant index */
struct bgen_vg;   /* variant genotype */

/* Variant metadata. */
struct bgen_var {
  struct bgen_string id;
  struct bgen_string rsid;
  struct bgen_string chrom;
  int position;
  int nalleles;
  struct bgen_string *allele_ids;
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
struct bgen_string *bgen_read_samples(struct bgen_file *bgen, int verbose);
/* Free array of sample identifications. */
void bgen_free_samples(const struct bgen_file *bgen,
                       struct bgen_string *samples);

/* Read variants metadata and generate variants index.

    Reading variants metadata (and generating the variants index) can be costly
    as it requires accessing chunks of data across the file. We therefore
    provide the functions
        - bgen_store_variants_metadata
        - bgen_load_variants_metadata
        - bgen_create_variants_metadata_file
    for storing and reading that information from an additional file. We refer
    to this file as variants metadata file.

    Note: remember to call `bgen_free_variants_metadata` and `bgen_free_index`
   after use.
*/
struct bgen_var *bgen_read_variants_metadata(struct bgen_file *bgen,
                                             struct bgen_vi **vi, int verbose);
void bgen_free_variants_metadata(const struct bgen_file *bgen,
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
