#ifndef BGEN_READER_H
#define BGEN_READER_H

#include <stdint.h>

#include "variant_id_block.h"

typedef struct BGenFile BGenFile;

BGenFile* bgen_reader_open(const char *filepath);
inti      bgen_reader_close(BGenFile *bgenfile);

inti      bgen_reader_nsamples(BGenFile *bgenfile);
inti      bgen_reader_nvariants(BGenFile *bgenfile);

// Get sample identification
inti      bgen_reader_sampleid(BGenFile *bgenfile,
                               inti      sample_idx,
                               byte    **id,
                               inti     *length);

// Get variant identification
inti bgen_reader_variantid(BGenFile *bgenfile,
                           inti      variant_idx,
                           byte    **id,
                           inti     *length);

// Get the variant rsid
inti bgen_reader_variant_rsid(BGenFile *bgenfile,
                              inti      variant_idx,
                              byte    **rsid,
                              inti     *length);

// Get identification of the variant's chromossome
inti bgen_reader_variant_chrom(BGenFile *bgenfile,
                               inti      variant_idx,
                               byte    **chrom,
                               inti     *length);

// Get variant's position
inti bgen_reader_variant_position(BGenFile *bgenfile,
                                  inti      variant_idx,
                                  inti     *position);

// Get the number of alleles a variant has
inti bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                  inti      variant_idx,
                                  inti     *nalleles);

// Returns the allele identification of a variant
inti bgen_reader_variant_alleleid(BGenFile *bgenfile,
                                  inti      variant_idx,
                                  inti      allele_idx,
                                  byte    **id,
                                  inti     *length);

inti bgen_reader_read_genotype(BGenFile *bgenfile,
                               inti      variant_idx,
                               inti    **ui_probs,
                               inti     *ploidy,
                               inti     *nalleles,
                               inti     *nbits);

inti bgen_reader_choose(inti n,
                        inti k);

inti bgen_reader_read_variantid_blocks(BGenFile        *bgenfile,
                                       VariantIdBlock **head_ref);

#endif /* end of include guard: BGEN_READER_H */
