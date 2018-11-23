#include "variant.h"
#include "bgen_file.h"
#include "mem.h"
#include "str.h"
#include <assert.h>

/* It assumes that the bgen file is open. */
int read_variant(struct bgen_file *bgen, struct bgen_var *v) {
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
