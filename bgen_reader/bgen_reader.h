#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdint.h>

typedef struct VariantIdData
{
    int64_t        nsamples;
    int64_t        id_length;
    unsigned char *id;
    int64_t        rsid_length;
    unsigned char *rsid;
    int64_t        chrom_length;
    unsigned char *chrom;
    int64_t        position;
    int64_t        nalleles;
    int64_t       *alleleid_length;
    unsigned char *alleles;
    struct VariantIdData *next;
} VariantIdData;

typedef struct BGenFile BGenFile;

BGenFile* bgen_reader_open(const char *filepath);
int64_t   bgen_reader_close(BGenFile *bgenfile);

int64_t   bgen_reader_nsamples(BGenFile *bgenfile);
int64_t   bgen_reader_nvariants(BGenFile *bgenfile);

// Get sample identification
int64_t   bgen_reader_sampleid(BGenFile       *bgenfile,
                               uint64_t        sample_idx,
                               unsigned char **id,
                               uint64_t       *length);

// Get variant identification
int64_t bgen_reader_variantid(BGenFile       *bgenfile,
                              uint64_t        variant_idx,
                              unsigned char **id,
                              uint64_t       *length);

// Get the variant rsid
int64_t bgen_reader_variant_rsid(BGenFile       *bgenfile,
                                 uint64_t        variant_idx,
                                 unsigned char **rsid,
                                 uint64_t       *length);

// Get identification of the variant's chromossome
int64_t bgen_reader_variant_chrom(BGenFile       *bgenfile,
                                  uint64_t        variant_idx,
                                  unsigned char **chrom,
                                  uint64_t       *length);

// Get variant's position
int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t *position);

// Get the number of alleles a variant has
int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t  variant_idx,
                                     uint64_t *nalleles);

// Returns the allele identification of a variant
int64_t bgen_reader_variant_alleleid(BGenFile       *bgenfile,
                                     uint64_t        variant_idx,
                                     uint64_t        allele_idx,
                                     unsigned char **id,
                                     uint64_t       *length);

int64_t bgen_reader_read_genotype(BGenFile  *bgenfile,
                                  uint64_t   variant_idx,
                                  uint32_t **ui_probs,
                                  uint64_t  *ploidy,
                                  uint64_t  *nalleles,
                                  uint64_t  *nbits);

int64_t bgen_reader_choose(uint64_t n,
                           uint64_t k);

// int64_t bgen_reader_read_all_variantid_data(BGenFile      *bgenfile,
//                                             VariantBlock **head_ref);

#endif /* end of include guard: BGEN_READER_H */
