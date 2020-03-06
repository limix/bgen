/* Read, alloc, and free variant metadata.
 */
#ifndef _BGEN_VARIANT_H
#define _BGEN_VARIANT_H

#include "bgen/bgen.h"

int next_variant(struct bgen_file *bgen, struct bgen_vm *v);
void init_metadata(struct bgen_vm *v);
struct bgen_vm *alloc_metadata(void);
void free_metadata(struct bgen_vm *v);

#endif /* _BGEN_VARIANT_H */
