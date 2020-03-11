#include "genotype.h"
#include "bgen/bgen.h"
#include "file.h"
#include "free.h"
#include "index.h"
#include "layout1.h"
#include "layout2.h"
#include "report.h"

void bgen_genotype_close(struct bgen_genotype const* genotype)
{
    free_c(genotype->plo_miss);
    free_c(genotype->chunk);
    free_c(genotype);
}

int bgen_genotype_read(struct bgen_genotype* genotype, double* probabilities)
{
    if (genotype->layout == 1) {
        bgen_layout1_read_genotype(genotype, probabilities);
    } else if (genotype->layout == 2) {
        bgen_layout2_read_genotype(genotype, probabilities);
    } else {
        bgen_error("unrecognized layout type %d", genotype->layout);
        return 1;
    }
    return 0;
}

uint16_t bgen_genotype_nalleles(struct bgen_genotype const* genotype)
{
    return genotype->nalleles;
}

bool bgen_genotype_missing(struct bgen_genotype const* genotype, int index)
{
    return genotype->plo_miss[index] >> 7;
}

uint8_t bgen_genotype_ploidy(struct bgen_genotype const* genotype, int index)
{
    return genotype->plo_miss[index] & 127;
}

uint8_t bgen_genotype_min_ploidy(struct bgen_genotype const* genotype)
{
    return genotype->min_ploidy;
}

uint8_t bgen_genotype_max_ploidy(struct bgen_genotype const* genotype)
{
    return genotype->max_ploidy;
}

unsigned bgen_genotype_ncombs(struct bgen_genotype const* genotype)
{
    return genotype->ncombs;
}

bool bgen_genotype_phased(struct bgen_genotype const* genotype) { return genotype->phased; }