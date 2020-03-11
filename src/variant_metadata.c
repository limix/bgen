#include "bgen/variant_metadata.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "report.h"
#include "str.h"
#include "variant_metadata.h"

struct bgen_vm* bgen_variant_metadata_create(void)
{
    struct bgen_vm* vm = malloc(sizeof(struct bgen_vm));
    vm->id = NULL;
    vm->rsid = NULL;
    vm->chrom = NULL;
    vm->position = 0;
    vm->nalleles = 0;
    vm->allele_ids = NULL;
    vm->genotype_offset = 0;
    return vm;
}

void bgen_variant_metadata_create_alleles(struct bgen_vm* vm, uint16_t nalleles)
{
    vm->allele_ids = malloc(sizeof(struct bgen_str*) * nalleles);

    for (uint16_t j = 0; j < nalleles; ++j) {
        vm->allele_ids[j] = NULL;
    }
}

struct bgen_vm* bgen_variant_metadata_begin(struct bgen_file* bgen, int* error)
{
    if (bgen_file_seek_variants_start(bgen)) {
        *error = 1;
        return NULL;
    }

    return bgen_variant_metadata_next(bgen, error);
}

struct bgen_vm* bgen_variant_metadata_next(struct bgen_file* bgen, int* error)
{
    struct bgen_vm* vm = bgen_variant_metadata_create();
    *error = 0;

    if (feof(bgen_file_stream(bgen)))
        return NULL;

    if (bgen_file_layout(bgen) == 1) {
        if (bgen_fseek(bgen_file_stream(bgen), 4, SEEK_CUR))
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

    for (uint16_t i = 0; i < vm->nalleles; ++i) {

        if ((vm->allele_ids[i] = bgen_str_fread(bgen_file_stream(bgen), 4)) == NULL)
            goto err;
    }

    int64_t offset = bgen_ftell(bgen_file_stream(bgen));
    if (offset < 0) {
        bgen_perror("could not ftell");
        goto err;
    }
    vm->genotype_offset = (uint64_t)offset;

    uint32_t length = 0;
    if (fread_ui32(bgen_file_stream(bgen), &length, 4))
        goto err;

    if (bgen_fseek(bgen_file_stream(bgen), length, SEEK_CUR)) {
        bgen_perror("could not jump to the next variant");
        goto err;
    }

    return vm;
err:
    bgen_variant_metadata_destroy(vm);
    *error = 1;
    return NULL;
}

struct bgen_vm* bgen_variant_metadata_end(struct bgen_file* bgen_file) { return NULL; }

void bgen_variant_metadata_destroy(struct bgen_vm const* vm)
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

    free_c(vm);
}
