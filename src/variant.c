#include "io.h"
#include "free.h"
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
    if (bgen_file_layout(bgen) == 1) {
        if (LONG_SEEK(bgen_file_stream(bgen), 4, SEEK_CUR))
            goto err;
    }

    if ((vm->id = bgen_str_fread_create(bgen_file_stream(bgen), 2)) == NULL)
        goto err;

    if ((vm->rsid = bgen_str_fread_create(bgen_file_stream(bgen), 2)) == NULL)
        goto err;

    if ((vm->chrom = bgen_str_fread_create(bgen_file_stream(bgen), 2)) == NULL)
        goto err;

    if (fread_int(bgen_file_stream(bgen), &vm->position, 4))
        goto err;

    if (bgen_file_layout(bgen) == 1)
        vm->nalleles = 2;
    else if (fread_int(bgen_file_stream(bgen), &vm->nalleles, 2))
        goto err;

    vm->allele_ids = malloc(vm->nalleles * sizeof(struct bgen_str));
    if (!vm->allele_ids)
        goto err;

    for (size_t i = 0; i < (size_t)vm->nalleles; ++i) {
        if (fread_str(bgen_file_stream(bgen), vm->allele_ids + i, 4))
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
    vm->id = NULL;
    vm->rsid = NULL;
    vm->chrom = NULL;
    vm->position = -1;
    vm->nalleles = -1;
}

struct bgen_vm *alloc_metadata(void)
{
    struct bgen_vm *v = malloc(sizeof(struct bgen_vm));
    if (v)
        init_metadata(v);
    return v;
}

void free_metadata(struct bgen_vm *vm)
{
    if (vm->id)
        bgen_str_free(vm->id);
    if (vm->rsid)
        bgen_str_free(vm->rsid);
    if (vm->chrom)
        bgen_str_free(vm->chrom);
    if (vm->allele_ids) {
        for (size_t i = 0; i < (size_t)vm->nalleles; ++i)
            bgen_str_free(vm->allele_ids + i);
    }
    free_c(vm->allele_ids);
    vm->allele_ids = NULL;
}
