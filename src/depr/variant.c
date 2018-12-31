#include "depr/variant.h"
#include "athr.h"
#include "depr/variants_index.h"
#include "file.h"
#include "mem.h"
#include "str.h"
#include <assert.h>

int read_next_variant_depr(struct bgen_file *bgen, struct bgen_var *v)
{
    assert(bgen);
    assert(bgen->file);
    assert(v);

    if (bgen->layout == 1) {
        if (fseek(bgen->file, 4, SEEK_CUR))
            goto err;
    }

    if (fread_str(bgen->file, &v->id, 2))
        goto err;

    if (fread_str(bgen->file, &v->rsid, 2))
        goto err;

    if (fread_str(bgen->file, &v->chrom, 2))
        goto err;

    if (fread_int(bgen->file, &v->position, 4))
        goto err;

    if (bgen->layout == 1)
        v->nalleles = 2;
    else if (fread_int(bgen->file, &v->nalleles, 2))
        goto err;

    v->allele_ids = dalloc(v->nalleles * sizeof(struct bgen_str));
    if (!v->allele_ids)
        goto err;

    for (size_t i = 0; i < v->nalleles; ++i) {
        if (fread_str(bgen->file, v->allele_ids + i, 4))
            goto err;
    }

    return 0;
err:
    free_nul(v->id.str);
    free_nul(v->rsid.str);
    free_nul(v->chrom.str);
    if (v->allele_ids) {
        for (size_t i = 0; i < v->nalleles; ++i)
            free_nul((v->allele_ids + i)->str);
    }
    free_nul(v->allele_ids);
    return 1;
}

BGEN_API struct bgen_var *bgen_read_metadata(struct bgen_file *bgen,
                                             struct bgen_vi **index, int verbose)
{
    assert(bgen);
    assert(index);

    struct bgen_var *variants = NULL;
    uint32_t length;
    size_t nvariants;
    struct athr *at = NULL;

    bopen_or_leave(bgen);

    fseek(bgen->file, bgen->variants_start, SEEK_SET);
    *index = new_variants_index(bgen);

    nvariants = bgen->nvariants;
    variants = dalloc(nvariants * sizeof(struct bgen_var));

    if (verbose)
        at = athr_create(nvariants, "Reading variants", ATHR_BAR);

    for (size_t i = 0; i < nvariants; ++i) {
        if (verbose)
            athr_consume(at, 1);

        if (read_next_variant_depr(bgen, variants + i))
            goto err;

        (*index)->start[i] = ftell(bgen->file);

        if (fread_ui32(bgen->file, &length, 4))
            goto err;

        if (fseek(bgen->file, length, SEEK_CUR)) {
            error("Could not jump to a variant.");
            goto err;
        }

        if ((variants + i)->nalleles > (*index)->max_nalleles)
            (*index)->max_nalleles = (variants + i)->nalleles;
    }

    if (verbose)
        athr_finish(at);

    fclose(bgen->file);

    return variants;

err:
    fclose_nul(bgen->file);
    if (*index)
        free_nul((*index)->start);
    free_nul(variants);
    return free_nul(*index);
}
