#ifndef BGEN_VARIANT_H_PRIVATE
#define BGEN_VARIANT_H_PRIVATE

#include <inttypes.h>

struct bgen_file;
struct bgen_variant;

struct bgen_variant* bgen_variant_create(void);
void bgen_variant_create_alleles(struct bgen_variant* variant, uint16_t nalleles);
struct bgen_variant* bgen_variant_begin(struct bgen_file* bgen_file, int* error);
struct bgen_variant* bgen_variant_next(struct bgen_file* bgen_file, int* error);
struct bgen_variant* bgen_variant_end(struct bgen_file const* bgen_file);
void                 bgen_variant_destroy(struct bgen_variant const* variant);

#endif
