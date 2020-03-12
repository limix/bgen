#include "bgen/variant.h"
#include "bgen/str.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "report.h"
#include "str.h"
#include "variant.h"

struct bgen_variant* bgen_variant_create(void)
{
    struct bgen_variant* variant = malloc(sizeof(struct bgen_variant));
    variant->id = NULL;
    variant->rsid = NULL;
    variant->chrom = NULL;
    variant->position = 0;
    variant->nalleles = 0;
    variant->allele_ids = NULL;
    variant->genotype_offset = 0;
    return variant;
}

void bgen_variant_create_alleles(struct bgen_variant* variant, uint16_t nalleles)
{
    variant->allele_ids = malloc(sizeof(struct bgen_str*) * nalleles);

    for (uint16_t j = 0; j < nalleles; ++j) {
        variant->allele_ids[j] = NULL;
    }
}

struct bgen_variant* bgen_variant_begin(struct bgen_file* bgen_file, int* error)
{
    if (bgen_file_seek_variants_start(bgen_file)) {
        *error = 1;
        return NULL;
    }

    return bgen_variant_next(bgen_file, error);
}

struct bgen_variant* bgen_variant_next(struct bgen_file* bgen_file, int* error)
{
    struct bgen_variant* vm = bgen_variant_create();
    *error = 0;

    if (feof(bgen_file_stream(bgen_file)))
        return NULL;

    if (bgen_file_layout(bgen_file) == 1) {
        if (bgen_fseek(bgen_file_stream(bgen_file), 4, SEEK_CUR))
            goto err;
    }

    if ((vm->id = bgen_str_fread(bgen_file_stream(bgen_file), 2)) == NULL)
        goto err;

    if ((vm->rsid = bgen_str_fread(bgen_file_stream(bgen_file), 2)) == NULL)
        goto err;

    if ((vm->chrom = bgen_str_fread(bgen_file_stream(bgen_file), 2)) == NULL)
        goto err;

    if (fread_ui32(bgen_file_stream(bgen_file), &vm->position, 4))
        goto err;

    if (bgen_file_layout(bgen_file) == 1)
        vm->nalleles = 2;
    else if (fread_ui16(bgen_file_stream(bgen_file), &vm->nalleles, 2))
        goto err;

    vm->allele_ids = malloc(vm->nalleles * sizeof(struct bgen_str*));
    for (uint16_t i = 0; i < vm->nalleles; ++i)
        vm->allele_ids[i] = NULL;

    for (uint16_t i = 0; i < vm->nalleles; ++i) {
        if ((vm->allele_ids[i] = bgen_str_fread(bgen_file_stream(bgen_file), 4)) == NULL)
            goto err;
    }

    int64_t offset = bgen_ftell(bgen_file_stream(bgen_file));
    if (offset < 0) {
        bgen_perror("could not ftell");
        goto err;
    }
    vm->genotype_offset = (uint64_t)offset;

    uint32_t length = 0;
    if (fread_ui32(bgen_file_stream(bgen_file), &length, 4))
        goto err;

    if (bgen_fseek(bgen_file_stream(bgen_file), length, SEEK_CUR)) {
        bgen_perror("could not jump to the next variant");
        goto err;
    }

    return vm;
err:
    bgen_variant_destroy(vm);
    *error = 1;
    return NULL;
}

struct bgen_variant* bgen_variant_end(struct bgen_file const* bgen_file) { return NULL; }

void bgen_variant_destroy(struct bgen_variant const* variant)
{
    if (variant->id)
        bgen_str_free(variant->id);

    if (variant->rsid)
        bgen_str_free(variant->rsid);

    if (variant->chrom)
        bgen_str_free(variant->chrom);

    if (variant->allele_ids) {
        for (uint16_t i = 0; i < variant->nalleles; ++i) {
            if (variant->allele_ids[i])
                bgen_str_free(variant->allele_ids[i]);
        }
        free_c(variant->allele_ids);
    }

    free_c(variant);
}
