#define BGEN_API_EXPORTS

#include "geno.h"
#include "bgen.h"
#include "depr/variants_index.h"
#include "file.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include <assert.h>

struct bgen_vg *create_vg()
{
    struct bgen_vg *vg = dalloc(sizeof(struct bgen_vg));
    vg->plo_miss = NULL;
    vg->chunk = NULL;
    vg->current_chunk = NULL;
    return vg;
}

int free_vg(struct bgen_vg *vg)
{
    if (vg) {
        vg->plo_miss = free_nul(vg->plo_miss);
        vg->current_chunk = vg->chunk = free_nul(vg->chunk);
    }
    return free_nul(vg) != NULL;
}

BGEN_API struct bgen_vg *bgen_open_genotype(struct bgen_file *bgen, struct bgen_vm *vm)
{
    struct bgen_vg *vg = create_vg();
    if (!vg) {
        error("Could not open genotype");
        goto err;
    }
    vg->vaddr = vm->vaddr;

    if (fseek(bgen->file, (long)vm->vaddr, SEEK_SET)) {
        perror_fmt("Could not seek a variant in %s", bgen->filepath);
        goto err;
    }

    struct bgen_vi vi;
    init_index(bgen, &vi);

    if (bgen->layout == 1) {
        bgen_read_probs_header_one(&vi, vg, bgen->file);
    } else if (bgen->layout == 2) {
        read_probs_header_two(&vi, vg, bgen->file);
    } else {
        error("Unrecognized layout type %d", bgen->layout);
        goto err;
    }

    free_nul(vi.filepath);
    free_nul(vi.start);
    return vg;

err:
    free_nul(vi.filepath);
    free_nul(vi.start);
    return free_nul(vg);
}

BGEN_API void bgen_close_genotype(struct bgen_vg *vg) { free_vg(vg); }

BGEN_API int bgen_read_genotype(struct bgen_file *bgen, struct bgen_vg *vg,
                                double *probs)
{
    if (bgen->layout == 1) {
        bgen_read_probs_one(vg, probs);
    } else if (bgen->layout == 2) {
        read_probs_two(vg, probs);
    } else {
        error("Unrecognized layout type %d", bgen->layout);
        return 1;
    }
    return 0;
}

BGEN_API int bgen_nalleles(const struct bgen_vg *vg) { return vg->nalleles; }

BGEN_API int bgen_missing(const struct bgen_vg *vg, size_t index)
{
    return vg->plo_miss[index] >> 7;
}

BGEN_API int bgen_ploidy(const struct bgen_vg *vg, size_t index)
{
    return vg->plo_miss[index] & 127;
}

BGEN_API int bgen_min_ploidy(const struct bgen_vg *vg) { return vg->min_ploidy; }

BGEN_API int bgen_max_ploidy(const struct bgen_vg *vg) { return vg->max_ploidy; }

BGEN_API int bgen_ncombs(const struct bgen_vg *vg) { return vg->ncombs; }

BGEN_API int bgen_phased(const struct bgen_vg *vg) { return vg->phased; }
