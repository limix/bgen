#ifndef _BGEN_VARIANT_H
#define _BGEN_VARIANT_H

#include "bgen.h"

int read_next_variant(struct bgen_file *bgen, struct bgen_var *v);
struct bgen_vm *alloc_metadata(void);

#endif /* end of include guard: _BGEN_VARIANT_H */
