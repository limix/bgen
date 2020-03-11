#ifndef BGEN_VARIANT_METADATA_H_PRIVATE
#define BGEN_VARIANT_METADATA_H_PRIVATE

#include <inttypes.h>

struct bgen_file;
struct bgen_variant_metadata;

struct bgen_variant_metadata* bgen_variant_metadata_create(void);
void bgen_variant_metadata_create_alleles(struct bgen_variant_metadata* vm, uint16_t nalleles);
struct bgen_variant_metadata* bgen_variant_metadata_begin(struct bgen_file* bgen_file,
                                                          int*              error);
struct bgen_variant_metadata* bgen_variant_metadata_next(struct bgen_file* bgen_file,
                                                         int*              error);
struct bgen_variant_metadata* bgen_variant_metadata_end(struct bgen_file const* bgen_file);
void bgen_variant_metadata_destroy(struct bgen_variant_metadata const* vm);

#endif
