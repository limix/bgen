#include "geno.h"
#include "bgen/bgen.h"
#include "file.h"
#include "free.h"
#include "index.h"
#include "layout1.h"
#include "layout2.h"
#include "report.h"

void bgen_genotype_close(struct bgen_genotype const* vg)
{
    free_c(vg->plo_miss);
    free_c(vg->chunk);
    free_c(vg);
}

int bgen_read_genotype(struct bgen_file const* bgen, struct bgen_genotype* vg, double* probs)
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

unsigned bgen_nalleles(struct bgen_genotype const* vg) { return vg->nalleles; }

bool bgen_missing(struct bgen_genotype const* vg, int index) { return vg->plo_miss[index] >> 7; }

unsigned bgen_ploidy(struct bgen_genotype const* vg, int index) { return vg->plo_miss[index] & 127; }

unsigned bgen_min_ploidy(struct bgen_genotype const* vg) { return vg->min_ploidy; }

unsigned bgen_max_ploidy(struct bgen_genotype const* vg) { return vg->max_ploidy; }

unsigned bgen_ncombs(struct bgen_genotype const* vg) { return vg->ncombs; }

bool bgen_phased(struct bgen_genotype const* vg) { return vg->phased; }
