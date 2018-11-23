#define BGEN_API_EXPORTS

#include "bgen.h"
#include "athr.h"
#include "bgen_err.h"
#include "bgen_file.h"
#include "index.h"
#include "io.h"
#include "layout/one.h"
#include "layout/two.h"
#include "mem.h"
#include "str.h"
#include "variant.h"
#include "variant_genotype.h"
#include "variants_index.h"
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
int bgen_read_header(struct bgen_file *bgen) {
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

BGEN_API struct bgen_file *bgen_open(const char *filepath) {
    assert(filepath);

    struct bgen_file *bgen = dalloc(sizeof(struct bgen_file));
    if (!bgen)
        goto err;

    bgen->samples_start = -1;
    bgen->variants_start = -1;
    bgen->file = NULL;

    bgen->filepath = strdup(filepath);

    bopen_or_leave(bgen);

    if (fread_int(bgen->file, &bgen->variants_start, 4))
        goto err;

    bgen->variants_start += 4;

    if (bgen_read_header(bgen)) {
        error("Could not read bgen header.");
        goto err;
    }

    /* if they actually exist */
    bgen->samples_start = ftell(bgen->file);

    if (fclose(bgen->file)) {
        perror_fmt("Could not close %s", filepath);
        goto err;
    }

    return bgen;

err:
    if (bgen) {
        fclose_nul(bgen->file);
        free_nul(bgen->filepath);
    }
    return free_nul(bgen);
}

BGEN_API void bgen_close(struct bgen_file *bgen) {
    if (bgen)
        free_nul(bgen->filepath);
    free_nul(bgen);
}

BGEN_API int bgen_nsamples(const struct bgen_file *bgen) {
    assert(bgen);
    return bgen->nsamples;
}

BGEN_API int bgen_nvariants(const struct bgen_file *bgen) {
    assert(bgen);
    return bgen->nvariants;
}

BGEN_API int bgen_contain_sample(const struct bgen_file *bgen) {
    assert(bgen);
    return bgen->contain_sample;
}

BGEN_API struct bgen_str *bgen_read_samples(struct bgen_file *bgen, int verbose) {
    assert(bgen);

    struct bgen_str *sample_ids = NULL;
    struct athr *at = NULL;

    bopen_or_leave(bgen);
    fseek(bgen->file, bgen->samples_start, SEEK_SET);

    if (bgen->contain_sample == 0) {
        error("This bgen file does not contain sample ids.");
        goto err;
    }

    sample_ids = dalloc(bgen->nsamples * sizeof(struct bgen_str));

    if (fseek(bgen->file, 8, SEEK_CUR))
        goto err;

    if (verbose)
        at = athr_create(bgen->nsamples, "Reading samples", ATHR_BAR);

    for (size_t i = 0; i < bgen->nsamples; ++i) {
        if (verbose)
            athr_consume(at, 1);

        if (fread_str(bgen->file, sample_ids + i, 2))
            goto err;
    }

    if (verbose)
        athr_finish(at);

    bgen->variants_start = ftell(bgen->file);
    fclose(bgen->file);

    return sample_ids;

err:
    fclose_nul(bgen->file);
    if (at)
        athr_finish(at);
    return free_nul(sample_ids);
}

BGEN_API void bgen_free_samples(const struct bgen_file *bgen, struct bgen_str *samples) {
    assert(bgen);
    assert(samples);

    if (bgen->contain_sample == 0)
        return;

    if (samples == NULL)
        return;

    for (size_t i = 0; i < bgen->nsamples; ++i)
        free_nul(samples[i].str);

    free(samples);
}

BGEN_API struct bgen_var *bgen_read_metadata(struct bgen_file *bgen,
                                             struct bgen_vi **index, int verbose) {
    assert(bgen);
    assert(index);

    struct bgen_var *variants = NULL;
    uint32_t length;
    size_t nvariants;
    struct athr *at = NULL;

    bopen_or_leave(bgen);

    fseek(bgen->file, bgen->variants_start, SEEK_SET);
    *index = new_variants_index(bgen);

    nvariants = bgen->nvariants;
    variants = dalloc(nvariants * sizeof(struct bgen_var));

    if (verbose)
        at = athr_create(nvariants, "Reading variants", ATHR_BAR);

    for (size_t i = 0; i < nvariants; ++i) {
        if (verbose)
            athr_consume(at, 1);

        if (read_variant(bgen, variants + i))
            goto err;

        (*index)->start[i] = ftell(bgen->file);

        if (fread_ui32(bgen->file, &length, 4))
            goto err;

        if (fseek(bgen->file, length, SEEK_CUR)) {
            error("Could not jump to a variant.");
            goto err;
        }

        if ((variants + i)->nalleles > (*index)->max_nalleles)
            (*index)->max_nalleles = (variants + i)->nalleles;
    }

    if (verbose)
        athr_finish(at);

    fclose(bgen->file);

    return variants;

err:
    fclose_nul(bgen->file);
    if (*index)
        free_nul((*index)->start);
    free_nul(variants);
    return free_nul(*index);
}

BGEN_API void bgen_free_variants_metadata(const struct bgen_file *bgen,
                                          struct bgen_var *variants) {

    assert(bgen);
    if (!variants)
        return;

    for (size_t i = 0; i < bgen->nvariants; ++i) {
        for (size_t j = 0; j < variants[i].nalleles; ++j) {
            free_nul(variants[i].allele_ids[j].str);
        }
        free_nul(variants[i].allele_ids);
        free_nul(variants[i].id.str);
        free_nul(variants[i].rsid.str);
        free_nul(variants[i].chrom.str);
    }
    free_nul(variants);
}

BGEN_API void bgen_free_index(struct bgen_vi *index) {
    if (index) {
        free_nul(index->filepath);
        free_nul(index->start);
    }
    free_nul(index);
}

BGEN_API int bgen_max_nalleles(struct bgen_vi *vi) {
    assert(vi);
    return vi->max_nalleles;
}

BGEN_API struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *vi, size_t index) {

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

BGEN_API int bgen_read_variant_genotype(struct bgen_vi *index, struct bgen_vg *vg,
                                        double *probs) {
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

BGEN_API void bgen_close_variant_genotype(struct bgen_vg *vg) {
    if (vg) {
        free_nul(vg->chunk);
        free_nul(vg->plo_miss);
    }
    free_nul(vg);
}

BGEN_API int bgen_nalleles(const struct bgen_vg *vg) {
    assert(vg);
    return vg->nalleles;
}

BGEN_API int bgen_missing(const struct bgen_vg *vg, size_t index) {
    assert(vg);
    return vg->plo_miss[index] >> 7;
}

BGEN_API int bgen_ploidy(const struct bgen_vg *vg, size_t index) {
    assert(vg);
    return vg->plo_miss[index] & 127;
}

BGEN_API int bgen_min_ploidy(const struct bgen_vg *vg) {
    assert(vg);
    return vg->min_ploidy;
}

BGEN_API int bgen_max_ploidy(const struct bgen_vg *vg) {
    assert(vg);
    return vg->max_ploidy;
}

BGEN_API int bgen_ncombs(const struct bgen_vg *vg) {
    assert(vg);
    return vg->ncombs;
}

BGEN_API int bgen_phased(const struct bgen_vg *vg) {
    assert(vg);
    return vg->phased;
}
