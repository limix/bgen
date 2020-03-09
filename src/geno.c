#include "geno.h"
#include "bgen/bgen.h"
#include "file.h"
#include "index.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include "free.h"
#include "report.h"
#include <assert.h>

struct bgen_vg* create_vg(void)
{
    struct bgen_vg* vg = malloc(sizeof(struct bgen_vg));
    vg->plo_miss = NULL;
    vg->chunk = NULL;
    vg->current_chunk = NULL;
    return vg;
}

int free_vg(struct bgen_vg* vg)
{
    if (vg) {
        free_c(vg->plo_miss);
        vg->plo_miss = NULL;
        free_c(vg->chunk);
        vg->current_chunk = vg->chunk = NULL;
    }
    free_c(vg);
    return 0;
}

struct bgen_vg* bgen_open_genotype(struct bgen_file* bgen, long vaddr)
{
    struct bgen_vg* vg = create_vg();
    if (!vg) {
        bgen_error("could not open genotype");
        goto err;
    }
    vg->vaddr = (OFF_T)vaddr;

    if (LONG_SEEK(bgen_file_stream(bgen), vaddr, SEEK_SET)) {
        bgen_perror("could not seek a variant in %s", bgen_file_filepath(bgen));
        goto err;
    }

    struct bgen_vi vi = BGEN_VI(bgen);

    if (bgen_file_layout(bgen) == 1) {
        bgen_read_probs_header_one(&vi, vg, bgen_file_stream(bgen));
    } else if (bgen_file_layout(bgen) == 2) {
        read_probs_header_two(&vi, vg, bgen_file_stream(bgen));
    } else {
        bgen_error("unrecognized layout type %d", bgen_file_layout(bgen));
        goto err;
    }

    return vg;

err:
    free_c(vg);
    return NULL;
}

void bgen_close_genotype(struct bgen_vg* vg) { free_vg(vg); }

int bgen_read_genotype(struct bgen_file const* bgen, struct bgen_vg* vg, double* probs)
{
    if (bgen_file_layout(bgen) == 1) {
        bgen_read_probs_one(vg, probs);
    } else if (bgen_file_layout(bgen) == 2) {
        read_probs_two(vg, probs);
    } else {
        bgen_error("Unrecognized layout type %d", bgen_file_layout(bgen));
        return 1;
    }
    return 0;
}

int bgen_nalleles(struct bgen_vg const* vg) { return vg->nalleles; }

int bgen_missing(struct bgen_vg const* vg, int index) { return vg->plo_miss[index] >> 7; }

int bgen_ploidy(struct bgen_vg const* vg, int index) { return vg->plo_miss[index] & 127; }

int bgen_min_ploidy(struct bgen_vg const* vg) { return vg->min_ploidy; }

int bgen_max_ploidy(struct bgen_vg const* vg) { return vg->max_ploidy; }

int bgen_ncombs(struct bgen_vg const* vg) { return vg->ncombs; }

int bgen_phased(struct bgen_vg const* vg) { return vg->phased; }
