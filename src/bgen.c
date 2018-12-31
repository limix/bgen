#define BGEN_API_EXPORTS

#include "bgen.h"
#include "athr.h"
#include "depr/variants_index.h"
#include "file.h"
#include "geno.h"
#include "index.h"
#include "io.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include "str.h"
#include "variant.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
