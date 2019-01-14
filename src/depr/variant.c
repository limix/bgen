#define BGEN_API_EXPORTS

#include "depr/variant.h"
#include "athr.h"
#include "depr/variants_index.h"
#include "file.h"
#include "geno.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include "str.h"
#include <assert.h>

BGEN_DEPRECATED_API int bgen_max_nalleles(struct bgen_vi *vi)
{
    assert(vi);
    return vi->max_nalleles;
}

BGEN_DEPRECATED_API void bgen_free_index(struct bgen_vi *index)
{
    if (index) {
        free_nul(index->filepath);
        free_nul(index->start);
    }
    free_nul(index);
}

BGEN_DEPRECATED_API void bgen_free_variants_metadata(const struct bgen_file *bgen,
                                                     struct bgen_var *variants)
{
    if (!variants)
        return;

    for (size_t i = 0; i < (size_t)bgen->nvariants; ++i) {
        for (size_t j = 0; j < (size_t)variants[i].nalleles; ++j) {
            free_nul(variants[i].allele_ids[j].str);
        }
        free_nul(variants[i].allele_ids);
        free_nul(variants[i].id.str);
        free_nul(variants[i].rsid.str);
        free_nul(variants[i].chrom.str);
    }
    free_nul(variants);
}

void fix_str(struct bgen_str *s)
{
    s->str = realloc(s->str, s->len + 1);
    s->str[s->len] = '\0';
}

int read_next_variant_depr(struct bgen_file *bgen, struct bgen_var *v)
{
    if (bgen->layout == 1) {
        if (LONG_SEEK(bgen->file, 4, SEEK_CUR))
            goto err;
    }

    if (fread_str(bgen->file, &v->id, 2))
        goto err;
    fix_str(&(v->id));

    if (fread_str(bgen->file, &v->rsid, 2))
        goto err;
    fix_str(&(v->rsid));

    if (fread_str(bgen->file, &v->chrom, 2))
        goto err;
    fix_str(&(v->chrom));

    if (fread_int(bgen->file, &v->position, 4))
        goto err;

    if (bgen->layout == 1)
        v->nalleles = 2;
    else if (fread_int(bgen->file, &v->nalleles, 2))
        goto err;

    v->allele_ids = dalloc(v->nalleles * sizeof(struct bgen_str));
    if (!v->allele_ids)
        goto err;

    for (size_t i = 0; i < (size_t)v->nalleles; ++i) {
        if (fread_str(bgen->file, v->allele_ids + i, 4))
            goto err;
        fix_str(v->allele_ids + i);
    }

    return 0;
err:
    free_nul(v->id.str);
    free_nul(v->rsid.str);
    free_nul(v->chrom.str);
    if (v->allele_ids) {
        for (size_t i = 0; i < (size_t)v->nalleles; ++i)
            free_nul((v->allele_ids + i)->str);
    }
    free_nul(v->allele_ids);
    return 1;
}

BGEN_DEPRECATED_API struct bgen_var *
bgen_read_variants_metadata(struct bgen_file *bgen, struct bgen_vi **index, int verbose)
{
    struct bgen_var *variants = NULL;
    uint32_t length;
    size_t nvariants;
    struct athr *at = NULL;

    LONG_SEEK(bgen->file, bgen->variants_start, SEEK_SET);
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

        if (LONG_SEEK(bgen->file, length, SEEK_CUR)) {
            error("Could not jump to a variant.");
            goto err;
        }

        if ((uint32_t)(variants + i)->nalleles > (*index)->max_nalleles)
            (*index)->max_nalleles = (variants + i)->nalleles;
    }

    if (verbose)
        athr_finish(at);

    return variants;

err:
    if (*index)
        free_nul((*index)->start);
    free_nul(variants);
    return free_nul(*index);
}

BGEN_DEPRECATED_API struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *vi,
                                                               size_t index)
{
    struct bgen_vg *vg = NULL;
    FILE *fp = NULL;

    if (!(fp = fopen(vi->filepath, "rb"))) {
        perror_fmt("Could not open %s", vi->filepath);
        goto err;
    }

    vg = dalloc(sizeof(struct bgen_vg));
    vg->variant_idx = index;
    vg->plo_miss = NULL;
    vg->chunk = NULL;

    if (LONG_SEEK(fp, vi->start[index], SEEK_SET)) {
        perror_fmt("Could not seek a variant in %s", vi->filepath);
        goto err;
    }

    if (vi->layout == 1) {
        bgen_read_probs_header_one(vi, vg, fp);
    } else if (vi->layout == 2) {
        read_probs_header_two(vi, vg, fp);
    } else {
        error("Unrecognized layout type.");
        goto err;
    }

    fclose(fp);
    return vg;

err:
    fclose_nul(fp);
    return free_nul(vg);
}

BGEN_DEPRECATED_API int bgen_read_variant_genotype(struct bgen_vi *index,
                                                   struct bgen_vg *vg, double *probs)
{
    if (index->layout == 1) {
        bgen_read_probs_one(vg, probs);
    } else if (index->layout == 2) {
        read_probs_two(vg, probs);
    } else {
        error("Unrecognized layout type.");
        return 1;
    }
    return 0;
}

BGEN_DEPRECATED_API void bgen_close_variant_genotype(struct bgen_vi *vi,
                                                     struct bgen_vg *vg)
{
    if (!vi)
        warn("Index is NULL");
    if (vg) {
        free_nul(vg->chunk);
        free_nul(vg->plo_miss);
    }
    free_nul(vg);
}
