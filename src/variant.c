#include "variant.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "str.h"

/* Fetch the next variant metadata from a bgen file.
 *
 * The user is responsible for freeing the allocated memory by calling
 * :func:`free_metadata`.
 *
 * Note
 * ----
 * It assumes that the bgen file is open.
 */
int next_variant(struct bgen_file* bgen, struct bgen_vm* vm)
{
    if (bgen_file_layout(bgen) == 1) {
        if (LONG_SEEK(bgen_file_stream(bgen), 4, SEEK_CUR))
            goto err;
    }

    if ((vm->id = bgen_str_fread(bgen_file_stream(bgen), 2)) == NULL)
        goto err;

    if ((vm->rsid = bgen_str_fread(bgen_file_stream(bgen), 2)) == NULL)
        goto err;

    if ((vm->chrom = bgen_str_fread(bgen_file_stream(bgen), 2)) == NULL)
        goto err;

    if (fread_ui32(bgen_file_stream(bgen), &vm->position, 4))
        goto err;

    if (bgen_file_layout(bgen) == 1)
        vm->nalleles = 2;
    else if (fread_ui16(bgen_file_stream(bgen), &vm->nalleles, 2))
        goto err;

    vm->allele_ids = malloc(vm->nalleles * sizeof(struct bgen_str*));
    for (int i = 0; i < vm->nalleles; ++i)
        vm->allele_ids[i] = NULL;

    if (!vm->allele_ids)
        goto err;

    for (size_t i = 0; i < (size_t)vm->nalleles; ++i) {

        if ((vm->allele_ids[i] = bgen_str_fread(bgen_file_stream(bgen), 4)) == NULL)
            goto err;
    }

    return 0;
err:
    free_metadata(vm);
    return 1;
}

void init_metadata(struct bgen_vm* vm)
{
    vm->genotype_offset = 0;
    vm->allele_ids = NULL;
    vm->id = NULL;
    vm->rsid = NULL;
    vm->chrom = NULL;
    vm->position = 0;
    vm->nalleles = 0;
}

struct bgen_vm* alloc_metadata(void)
{
    struct bgen_vm* v = malloc(sizeof(struct bgen_vm));
    if (v)
        init_metadata(v);
    return v;
}

void free_metadata(struct bgen_vm* vm)
{
    if (vm->id)
        bgen_str_free(vm->id);

    if (vm->rsid)
        bgen_str_free(vm->rsid);

    if (vm->chrom)
        bgen_str_free(vm->chrom);

    if (vm->allele_ids) {
        for (size_t i = 0; i < (size_t)vm->nalleles; ++i) {
            if (vm->allele_ids[i])
                bgen_str_free(vm->allele_ids[i]);
        }
        free_c(vm->allele_ids);
    }
    vm->allele_ids = NULL;
}
