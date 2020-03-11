#ifndef BGEN_VARIANT_H
#define BGEN_VARIANT_H

#include "bgen/bgen.h"

int             next_variant(struct bgen_file* bgen, struct bgen_variant_metadata* v);
void            init_metadata(struct bgen_variant_metadata* v);
struct bgen_variant_metadata* alloc_metadata(void);
void            free_metadata(struct bgen_variant_metadata* v);

#endif
