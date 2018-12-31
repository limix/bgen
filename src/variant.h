#ifndef _BGEN_VARIANT_H
#define _BGEN_VARIANT_H

#include "bgen.h"

int read_next_variant(struct bgen_file *bgen, struct bgen_vm *v);
struct bgen_vm *alloc_metadata(void);
void free_metadata(struct bgen_vm *v);

#endif /* _BGEN_VARIANT_H */
