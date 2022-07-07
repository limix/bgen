#include "bgen/variant.h"
#include "bgen/bstring.h"
#include "bstring.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "report.h"
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
    variant->allele_ids = malloc(sizeof(struct bgen_string*) * nalleles);
    for (uint16_t j = 0; j < nalleles; ++j)
        variant->allele_ids[j] = NULL;
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
    *error = 0;

    struct bgen_variant* v = bgen_variant_create();

    if (bgen_file_layout(bgen_file) == 1) {
        if (bgen_fseek(bgen_file_stream(bgen_file), 4, SEEK_CUR))
            goto err;
    }
    if (bgen_file_layout(bgen_file) != 2) {
        bgen_error("unknown layout %d", bgen_file_layout(bgen_file));
        goto err;
    }

    if ((v->id = bgen_string_fread(bgen_file_stream(bgen_file), 2)) == NULL) {
        if (feof(bgen_file_stream(bgen_file))) {
            bgen_variant_destroy(v);
            return NULL;
        }
        bgen_perror("could not read variant id");
        goto err;
    }

    if ((v->rsid = bgen_string_fread(bgen_file_stream(bgen_file), 2)) == NULL) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read variant rsid");
        goto err;
    }

    if ((v->chrom = bgen_string_fread(bgen_file_stream(bgen_file), 2)) == NULL) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read variant chrom");
        goto err;
    }

    if (fread(&v->position, sizeof(v->position), 1, bgen_file_stream(bgen_file)) != 1) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read variant position");
        goto err;
    }

    if (bgen_file_layout(bgen_file) == 1)
        v->nalleles = 2;
    else if (fread(&v->nalleles, sizeof(v->nalleles), 1, bgen_file_stream(bgen_file)) != 1) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read number of alleles");
        goto err;
    }

    v->allele_ids = malloc(v->nalleles * sizeof(struct bgen_string*));
    for (uint16_t i = 0; i < v->nalleles; ++i)
        v->allele_ids[i] = NULL;

    for (uint16_t i = 0; i < v->nalleles; ++i) {
        if ((v->allele_ids[i] = bgen_string_fread(bgen_file_stream(bgen_file), 4)) == NULL) {
            bgen_perror_eof(bgen_file_stream(bgen_file), "could not read allele id");
            goto err;
        }
    }

    int64_t offset = bgen_ftell(bgen_file_stream(bgen_file));
    if (offset < 0) {
        bgen_perror("could not ftell");
        goto err;
    }
    v->genotype_offset = (uint64_t)offset;

    uint32_t length = 0;
    if (fread(&length, sizeof(length), 1, bgen_file_stream(bgen_file)) != 1) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read length to skip");
        goto err;
    }

    if (bgen_fseek(bgen_file_stream(bgen_file), length, SEEK_CUR)) {
        bgen_perror("could not jump to the next variant");
        goto err;
    }

    return v;
err:
    bgen_variant_destroy(v);
    *error = 1;
    return NULL;
}

struct bgen_variant* bgen_variant_end(struct bgen_file const* bgen_file)
{
    (void)bgen_file;
    return NULL;
}

void bgen_variant_destroy(struct bgen_variant const* variant)
{
    if (variant->id)
        bgen_string_destroy(variant->id);

    if (variant->rsid)
        bgen_string_destroy(variant->rsid);

    if (variant->chrom)
        bgen_string_destroy(variant->chrom);

    if (variant->allele_ids) {
        for (uint16_t i = 0; i < variant->nalleles; ++i) {
            if (variant->allele_ids[i])
                bgen_string_destroy(variant->allele_ids[i]);
        }
        bgen_free(variant->allele_ids);
    }

    bgen_free(variant);
}
