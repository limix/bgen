#include "bgen/variant_metadata.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "str.h"
#include "variant_metadata.h"

static struct bgen_vm* variant_metadata_create(void);
static void            variant_metadata_destroy(struct bgen_vm const* vm);

struct bgen_vm* bgen_variant_metadata_begin(struct bgen_file* bgen, int* error)
{
    if (bgen_file_seek_variants_start(bgen)) {
        *error = 1;
        return NULL;
    }

    return bgen_variant_metadata_next(bgen, error);
}

/* Fetch the next variant metadata from a bgen file.
 */
struct bgen_vm* bgen_variant_metadata_next(struct bgen_file* bgen, int* error)
{
    struct bgen_vm* vm = variant_metadata_create();

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

    return vm;
err:
    variant_metadata_destroy(vm);
    *error = 1;
    return NULL;
}

static struct bgen_vm* variant_metadata_create(void)
{
    struct bgen_vm* vm = malloc(sizeof(struct bgen_vm));
    vm->vaddr = -1;
    vm->id = NULL;
    vm->rsid = NULL;
    vm->chrom = NULL;
    vm->position = 0;
    vm->nalleles = 0;
    vm->allele_ids = NULL;
    return vm;
}

static void variant_metadata_destroy(struct bgen_vm const* vm)
{
    if (vm->id)
        bgen_str_free(vm->id);

    if (vm->rsid)
        bgen_str_free(vm->rsid);

    if (vm->chrom)
        bgen_str_free(vm->chrom);

    if (vm->allele_ids) {
        for (uint16_t i = 0; i < vm->nalleles; ++i) {
            if (vm->allele_ids[i])
                bgen_str_free(vm->allele_ids[i]);
        }
        free_c(vm->allele_ids);
    }
}
