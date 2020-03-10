#ifndef BGEN_VARIANT_METADATA_H_PRIVATE
#define BGEN_VARIANT_METADATA_H_PRIVATE

#include <inttypes.h>

struct bgen_file;
struct bgen_vm;

struct bgen_vm* bgen_variant_metadata_create(void);
void            bgen_variant_metadata_create_alleles(struct bgen_vm* vm, uint16_t nalleles);
struct bgen_vm* bgen_variant_metadata_begin(struct bgen_file* bgen_file, int* error);
struct bgen_vm* bgen_variant_metadata_next(struct bgen_file* bgen_file, int* error);
struct bgen_vm* bgen_variant_metadata_end(struct bgen_file* bgen_file);
void            bgen_variant_metadata_destroy(struct bgen_vm const* vm);

#endif
