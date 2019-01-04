#include "variant.h"
#include "file.h"
#include "mem.h"
#include "str.h"
#include <assert.h>

/* Fetch the next variant metadata from a bgen file.
 *
 * The user is responsible for freeing the allocated memory by calling
 * :func:`free_metadata`.
 *
 * Note
 * ----
 * It assumes that the bgen file is open.
 */
int next_variant(struct bgen_file *bgen, struct bgen_vm *vm)
{
    if (bgen->layout == 1) {
        if (fseek(bgen->file, 4, SEEK_CUR))
            goto err;
    }

    if (fread_str(bgen->file, &vm->id, 2))
        goto err;

    if (fread_str(bgen->file, &vm->rsid, 2))
        goto err;

    if (fread_str(bgen->file, &vm->chrom, 2))
        goto err;

    if (fread_int(bgen->file, &vm->position, 4))
        goto err;

    if (bgen->layout == 1)
        vm->nalleles = 2;
    else if (fread_int(bgen->file, &vm->nalleles, 2))
        goto err;

    vm->allele_ids = dalloc(vm->nalleles * sizeof(struct bgen_str));
    if (!vm->allele_ids)
        goto err;

    for (size_t i = 0; i < (size_t)vm->nalleles; ++i) {
        if (fread_str(bgen->file, vm->allele_ids + i, 4))
            goto err;
    }

    return 0;
err:
    free_metadata(vm);
    return 1;
}

void init_metadata(struct bgen_vm *vm)
{
    vm->vaddr = -1;
    vm->allele_ids = NULL;
    vm->id.str = NULL;
    vm->rsid.str = NULL;
    vm->chrom.str = NULL;
    vm->position = -1;
    vm->nalleles = -1;
}

struct bgen_vm *alloc_metadata(void)
{
    struct bgen_vm *v = dalloc(sizeof(struct bgen_vm));
    if (v)
        init_metadata(v);
    return v;
}

void free_metadata(struct bgen_vm *vm)
{
    free_str(&vm->id);
    free_str(&vm->rsid);
    free_str(&vm->chrom);
    if (vm->allele_ids) {
        for (size_t i = 0; i < (size_t)vm->nalleles; ++i)
            free_str(vm->allele_ids + i);
    }
    vm->allele_ids = free_nul(vm->allele_ids);
}
