#define BGEN_API_EXPORTS

#include "geno.h"
#include "bgen.h"
#include "depr/variants_index.h"
#include "file.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include <assert.h>

BGEN_API struct bgen_vg *bgen_open_genotype(struct bgen_vi *vi, size_t index)
{

    assert(vi);
    assert(vi->filepath);

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

    if (fseek(fp, (long)vi->start[index], SEEK_SET)) {
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

BGEN_API int bgen_read_genotype(struct bgen_vi *index, struct bgen_vg *vg,
                                double *probs)
{
    assert(index);
    assert(vg);
    assert(probs);

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

BGEN_API void bgen_close_genotype(struct bgen_vg *vg)
{
    if (vg) {
        free_nul(vg->chunk);
        free_nul(vg->plo_miss);
    }
    free_nul(vg);
}

BGEN_API int bgen_nalleles(const struct bgen_vg *vg)
{
    assert(vg);
    return vg->nalleles;
}

BGEN_API int bgen_missing(const struct bgen_vg *vg, size_t index)
{
    assert(vg);
    return vg->plo_miss[index] >> 7;
}

BGEN_API int bgen_ploidy(const struct bgen_vg *vg, size_t index)
{
    assert(vg);
    return vg->plo_miss[index] & 127;
}

BGEN_API int bgen_min_ploidy(const struct bgen_vg *vg)
{
    assert(vg);
    return vg->min_ploidy;
}

BGEN_API int bgen_max_ploidy(const struct bgen_vg *vg)
{
    assert(vg);
    return vg->max_ploidy;
}

BGEN_API int bgen_ncombs(const struct bgen_vg *vg)
{
    assert(vg);
    return vg->ncombs;
}

BGEN_API int bgen_phased(const struct bgen_vg *vg)
{
    assert(vg);
    return vg->phased;
}
