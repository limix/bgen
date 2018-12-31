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

/*
 * Read the header block defined as follows:
 *
 *   header length: 4 bytes
 *   number of variants: 4 bytes
 *   number of samples: 4 bytes
 *   magic number: 4 bytes
 *   unused space: header length minus 20 bytes
 *   bgen flags: 4 bytes
 */
int bgen_read_header(struct bgen_file *bgen)
{
    assert(bgen);

    uint32_t header_length;
    uint32_t magic_number;
    uint32_t flags;

    if (fread_ui32(bgen->file, &header_length, 4)) {
        error("Could not read the header length.");
        return 1;
    }

    if (fread_int(bgen->file, &bgen->nvariants, 4)) {
        error("Could not read the number of variants.");
        return 1;
    }

    if (fread_int(bgen->file, &bgen->nsamples, 4)) {
        error("Could not read the number of samples.");
        return 1;
    }

    if (fread_ui32(bgen->file, &magic_number, 4)) {
        error("Could not read the magic number.");
        return 1;
    }

    if (magic_number != 1852139362) {
        error("This is not a BGEN file: magic number mismatch.");
        return 1;
    }

    if (fseek(bgen->file, header_length - 20, SEEK_CUR)) {
        error("Fseek error while reading a BGEN file.");
        return 1;
    }

    if (fread_ui32(bgen->file, &flags, 4)) {
        error("Could not read the bgen flags.");
        return 1;
    }

    bgen->compression = flags & 3;
    bgen->layout = (flags & (15 << 2)) >> 2;
    bgen->contain_sample = (flags & (1 << 31)) >> 31;

    return 0;
}

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
        bgen_read_probs_header_two(vi, vg, fp);
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
        bgen_read_probs_two(vg, probs);
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
