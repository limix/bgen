#include "genotype.h"
#include "bgen/genotype.h"
#include "free.h"
#include "layout1.h"
#include "layout2.h"
#include "report.h"

void bgen_genotype_close(struct bgen_genotype const* genotype)
{
    bgen_free(genotype->ploidy_missingness);
    bgen_free(genotype->chunk);
    bgen_free(genotype);
}

int bgen_genotype_read(struct bgen_genotype* genotype, double* probabilities)
{
    if (genotype->layout == 1) {
        bgen_layout1_read_genotype64(genotype, probabilities);
    } else if (genotype->layout == 2) {
        bgen_layout2_read_genotype64(genotype, probabilities);
    } else {
        bgen_error("unrecognized layout type %d", genotype->layout);
        return 1;
    }
    return 0;
}

int bgen_genotype_read64(struct bgen_genotype* genotype, double* probabilities)
{
    return bgen_genotype_read(genotype, probabilities);
}

int bgen_genotype_read32(struct bgen_genotype* genotype, float* probabilities)
{
    if (genotype->layout == 1) {
        bgen_layout1_read_genotype32(genotype, probabilities);
    } else if (genotype->layout == 2) {
        bgen_layout2_read_genotype32(genotype, probabilities);
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

bool bgen_genotype_missing(struct bgen_genotype const* genotype, uint32_t index)
{
    return genotype->ploidy_missingness[index] >> 7;
}

uint8_t bgen_genotype_ploidy(struct bgen_genotype const* genotype, uint32_t index)
{
    return genotype->ploidy_missingness[index] & 127;
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
