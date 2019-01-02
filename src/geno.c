#define BGEN_API_EXPORTS

#include "geno.h"
#include "bgen.h"
#include "depr/variants_index.h"
#include "file.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include <assert.h>

#if BGEN_OPEN_GENOTYPE == 3
BGEN_API struct bgen_vg *bgen_open_genotype(struct bgen_file *bgen, struct bgen_vm *vm)
{
    bopen_or_leave(bgen);

    /* if (!(fp = fopen(bgen->filepath, "rb"))) { */
    /*     perror_fmt("Could not open %s", vm->filepath); */
    /*     goto err; */
    /* } */

    struct bgen_vg *vg = dalloc(sizeof(struct bgen_vg));
    vg->variant_idx = index;
    vg->plo_miss = NULL;
    vg->chunk = NULL;

    if (fseek(fp, (long)vm->vaddr, SEEK_SET)) {
        perror_fmt("Could not seek a variant in %s", vm->filepath);
        goto err;
    }

    if (bgen->layout == 1) {
        bgen_read_probs_header_one(vm, vg, fp);
    } else if (bgen->layout == 2) {
        read_probs_header_two(vm, vg, fp);
    } else {
        error("Unrecognized layout type.");
        goto err;
    }

    fclose(fp);
    return vg;

err:
    fclose_nul(bgen->fp);
    bgen->file = NULL;
    return free_nul(vg);
}
#endif

BGEN_DEPRECATED BGEN_API int bgen_read_genotype(struct bgen_vi *index,
                                                struct bgen_vg *vg, double *probs)
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
