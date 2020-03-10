#ifndef BGEN_VARIANT_METADATA_H_PRIVATE
#define BGEN_VARIANT_METADATA_H_PRIVATE

struct bgen_file;
struct bgen_vm;

struct bgen_vm* bgen_variant_metadata_begin(struct bgen_file* bgen_file, int* error);
struct bgen_vm* bgen_variant_metadata_next(struct bgen_file* bgen_file, int* error);
struct bgen_vm* bgen_variant_metadata_end(struct bgen_file* bgen_file);

#endif
