#include "geno.h"
#include "bgen.h"
#include "depr/variants_index.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include <stdio.h>

#if BGEN_OPEN_GENOTYPE == 1
BGEN_API struct bgen_vg *bgen_open_genotype(struct bgen_vi *vi, size_t index)
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
#endif
