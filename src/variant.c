#define BGEN_API_EXPORTS

#include "variant.h"
#include "file.h"
#include "mem.h"
#include "str.h"
#include <assert.h>

/* It assumes that the bgen file is open. */
int read_next_variant(struct bgen_file *bgen, struct bgen_var *v)
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

struct bgen_vm *alloc_metadata(void)
{
    struct bgen_vm *v = dalloc(sizeof(struct bgen_vm));
    v->vaddr = -1;
    v->allele_ids = NULL;
    v->id.str = NULL;
    v->rsid.str = NULL;
    v->chrom.str = NULL;
    v->position = -1;
    v->nalleles = -1;
    return v;
}

BGEN_API void bgen_free_metadata(struct bgen_vm *v)
{
    free_str(&v->id);
    free_str(&v->rsid);
    free_str(&v->chrom);
    if (v->allele_ids) {
        for (size_t i = 0; i < v->nalleles; ++i)
            free_str(v->allele_ids + i);
    }
    free_nul(v->allele_ids);
}
