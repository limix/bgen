#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdint.h>
#include "byte.h"

typedef struct BGenFile BGenFile;

BGenFile* bgen_reader_open(char *filename);
int64_t   bgen_reader_close(BGenFile *bgenfile);

int64_t   bgen_reader_layout(BGenFile *bgenfile);
int64_t   bgen_reader_compression(BGenFile *bgenfile);
int64_t   bgen_reader_sampleids(BGenFile *bgenfile);
int64_t   bgen_reader_nsamples(BGenFile *bgenfile);
int64_t   bgen_reader_nvariants(BGenFile *bgenfile);

// Get sample identification
int64_t   bgen_reader_sampleid(BGenFile *bgenfile,
                               uint64_t  sample_idx,
                               BYTE    **id,
                               uint64_t *length);

// Get variant identification
int64_t bgen_reader_variantid(BGenFile *bgenfile,
                              uint64_t  variant_idx,
                              BYTE    **id,
                              uint64_t *length);

// Get the variant rsid
int64_t bgen_reader_variant_rsid(BGenFile *bgenfile,
                                 uint64_t  variant_idx,
                                 BYTE    **rsid,
                                 uint64_t *length);

// Get identification of the variant's chromossome
int64_t bgen_reader_variant_chrom(BGenFile *bgenfile,
                                  uint64_t  variant_idx,
                                  BYTE    **chrom,
                                  uint64_t *length);

// Get variant's position
int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t *position);

// Get the number of alleles a variant has
int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t *nalleles);

// Returns the allele identification of a variant
int64_t bgen_reader_variant_alleleid(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t  allele_idx,
                                     BYTE    **id,
                                     uint64_t *length);

int64_t bgen_reader_read_genotype(BGenFile *bgenfile,
                                  uint64_t  variant_idx,
                                  uint32_t **ui_probs);

#endif /* end of include guard: BGEN_READER_H */
