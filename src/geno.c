#include "geno.h"
#include "bgen/bgen.h"
#include "file.h"
#include "free.h"
#include "index.h"
#include "layout1.h"
#include "layout2.h"
#include "report.h"

static struct bgen_vg* create_vg(void);

struct bgen_vg* bgen_open_genotype(struct bgen_file const* bgen, long const vaddr)
{
    struct bgen_vg* vg = create_vg();
    vg->vaddr = (OFF_T)vaddr;

    if (LONG_SEEK(bgen_file_stream(bgen), vaddr, SEEK_SET)) {
        bgen_perror("could not seek a variant in %s", bgen_file_filepath(bgen));
        free_c(vg);
        return NULL;
    }

    struct bgen_vi vi = BGEN_VI(bgen);

    if (bgen_file_layout(bgen) == 1) {
        bgen_layout1_read_header(&vi, vg, bgen_file_stream(bgen));
    } else if (bgen_file_layout(bgen) == 2) {
        bgen_layout2_read_header(&vi, vg, bgen_file_stream(bgen));
    } else {
        bgen_error("unrecognized layout type %d", bgen_file_layout(bgen));
        free_c(vg);
        return NULL;
    }

    return vg;
}

void bgen_close_genotype(struct bgen_vg const* vg)
{
    free_c(vg->plo_miss);
    free_c(vg->chunk);
    free_c(vg);
}

int bgen_read_genotype(struct bgen_file const* bgen, struct bgen_vg* vg, double* probs)
{
    if (bgen_file_layout(bgen) == 1) {
        bgen_layout1_read_genotype(vg, probs);
    } else if (bgen_file_layout(bgen) == 2) {
        bgen_layout2_read_genotype(vg, probs);
    } else {
        bgen_error("unrecognized layout type %d", bgen_file_layout(bgen));
        return 1;
    }
    return 0;
}

unsigned bgen_nalleles(struct bgen_vg const* vg) { return vg->nalleles; }

bool bgen_missing(struct bgen_vg const* vg, int index) { return vg->plo_miss[index] >> 7; }

unsigned bgen_ploidy(struct bgen_vg const* vg, int index) { return vg->plo_miss[index] & 127; }

unsigned bgen_min_ploidy(struct bgen_vg const* vg) { return vg->min_ploidy; }

unsigned bgen_max_ploidy(struct bgen_vg const* vg) { return vg->max_ploidy; }

unsigned bgen_ncombs(struct bgen_vg const* vg) { return vg->ncombs; }

bool bgen_phased(struct bgen_vg const* vg) { return vg->phased; }

static struct bgen_vg* create_vg(void)
{
    struct bgen_vg* vg = malloc(sizeof(struct bgen_vg));
    vg->plo_miss = NULL;
    vg->chunk = NULL;
    vg->current_chunk = NULL;
    return vg;
}
