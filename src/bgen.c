#define BGEN_API_EXPORTS

#include "bgen.h"
#include "athr.h"
#include "bgen_err.h"
#include "bgen_file.h"
#include "index.h"
#include "layout/one.h"
#include "layout/two.h"
#include "util/file.h"
#include "util/int.h"
#include "util/mem.h"
#include "util/print.h"
#include "util/str.h"
#include "variant_genotype.h"
#include "variants_index.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bopen_or_leave(BGEN)                                                            \
    if (!(BGEN->file = fopen(BGEN->filepath, "rb"))) {                                  \
        PERROR("Could not open bgen file %s", BGEN->filepath);                          \
        goto err;                                                                       \
    }

int bgen_read_header(struct bgen_file *bgen) {
    uint32_t header_length;
    uint32_t magic_number;
    uint32_t flags;

    if (uint32_t_fread(bgen->file, &header_length, 4)) {
        error("Could not read the header length.");
        return 1;
    }

    if (int_fread(bgen->file, &bgen->nvariants, 4)) {
        error("Could not read the number of variants.");
        return 1;
    }

    if (int_fread(bgen->file, &bgen->nsamples, 4)) {
        error("Could not read the number of samples.");
        return 1;
    }

    if (uint32_t_fread(bgen->file, &magic_number, 4)) {
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

    if (uint32_t_fread(bgen->file, &flags, 4)) {
        error("Could not read the bgen flags.");
        return 1;
    }

    bgen->compression = flags & 3;
    bgen->layout = (flags & (15 << 2)) >> 2;
    bgen->sample_ids_presence = (flags & (1 << 31)) >> 31;

    return 0;
}

/**
 * filepath
 *  File path to the bgen file.
 */
BGEN_API struct bgen_file *bgen_open(const char *filepath) {

    struct bgen_file *bgen = ALLOC(sizeof(struct bgen_file));
    bgen->samples_start = -1;
    bgen->variants_start = -1;
    bgen->file = NULL;

    bgen->filepath = strdup(filepath);

    bopen_or_leave(bgen);

    if (int_fread(bgen->file, &bgen->variants_start, 4))
        goto err;

    bgen->variants_start += 4;

    if (bgen_read_header(bgen)) {
        fprintf(stderr, "Could not read bgen header.\n");
        goto err;
    }

    if (bgen->sample_ids_presence == 0) {
        bgen->samples_start = ftell(bgen->file);
    } else {
        bgen->samples_start = ftell(bgen->file);

        if (bgen->samples_start == EOF) {
            perror("Could not find sample blocks");
            goto err;
        }
    }

    if (fclose(bgen->file)) {
        PERROR("Could not close %s", filepath);
        goto err;
    }

    return bgen;

err:
    FCLOSE(bgen->file);
    if (bgen)
        FREE(bgen->filepath);
    FREE(bgen);
    return NULL;
}

BGEN_API void bgen_close(struct bgen_file *bgen) {
    if (bgen)
        FREE(bgen->filepath);
    FREE(bgen);
}

BGEN_API int bgen_nsamples(const struct bgen_file *bgen) { return bgen->nsamples; }

BGEN_API int bgen_nvariants(const struct bgen_file *bgen) { return bgen->nvariants; }

BGEN_API int bgen_sample_ids_presence(const struct bgen_file *bgen) {
    return bgen->sample_ids_presence;
}

BGEN_API struct bgen_str *bgen_read_samples(struct bgen_file *bgen, int verbose) {
    struct bgen_str *sample_ids;
    struct athr *at = NULL;

    bopen_or_leave(bgen);

    fseek(bgen->file, bgen->samples_start, SEEK_SET);

    if (bgen->sample_ids_presence == 0) {
        fclose(bgen->file);
        return NULL;
    }

    sample_ids = ALLOC(bgen->nsamples * sizeof(struct bgen_str));

    if (fseek(bgen->file, 8, SEEK_CUR))
        goto err;

    if (verbose)
        at = athr_create(bgen->nsamples, "Reading samples", ATHR_BAR);

    for (size_t i = 0; i < bgen->nsamples; ++i) {
        if (verbose)
            athr_consume(at, 1);

        if (str_fread(bgen->file, sample_ids + i, 2))
            goto err;
    }

    if (verbose)
        athr_finish(at);

    bgen->variants_start = ftell(bgen->file);

    if (fclose(bgen->file)) {
        PERROR("Could not close %s", bgen->filepath);
        goto err;
    }

    return sample_ids;

err:
    FCLOSE(bgen->file);
    FREE(sample_ids);
    if (at)
        athr_finish(at);
    return NULL;
}

BGEN_API void bgen_free_samples(const struct bgen_file *bgen, struct bgen_str *samples) {
    size_t i;

    if (bgen->sample_ids_presence == 0)
        return;

    if (samples == NULL)
        return;

    for (i = 0; i < (size_t)bgen->nsamples; ++i) {
        free(samples[i].str);
    }
    free(samples);
}

int bgen_read_variant(struct bgen_file *bgen, struct bgen_var *v) {

    if (bgen->layout == 1) {
        if (fseek(bgen->file, 4, SEEK_CUR))
            return 1;
    }

    if (str_fread(bgen->file, &v->id, 2))
        return 1;

    if (str_fread(bgen->file, &v->rsid, 2))
        return 1;

    if (str_fread(bgen->file, &v->chrom, 2))
        return 1;

    if (int_fread(bgen->file, &v->position, 4))
        return 1;

    if (bgen->layout == 1)
        v->nalleles = 2;
    else if (int_fread(bgen->file, &v->nalleles, 2))
        return 1;

    v->allele_ids = ALLOC(v->nalleles * sizeof(struct bgen_str));
    if (!v->allele_ids)
        return 1;

    for (size_t i = 0; i < v->nalleles; ++i) {
        if (str_fread(bgen->file, v->allele_ids + i, 4))
            return 1;
    }

    return 0;
}

BGEN_API struct bgen_var *bgen_read_metadata(struct bgen_file *bgen,
                                             struct bgen_vi **index, int verbose) {
    struct bgen_var *variants;
    uint32_t length;
    size_t i, nvariants;
    struct athr *at = NULL;

    variants = NULL;
    bopen_or_leave(bgen);

    if (fseek(bgen->file, bgen->variants_start, SEEK_SET)) {
        perror("Could not jump to variants start ");
        goto err;
    }

    *index = new_variants_index(bgen);

    nvariants = bgen->nvariants;
    variants = malloc(nvariants * sizeof(struct bgen_var));

    if (verbose) {
        at = athr_create(nvariants, "Reading variants", ATHR_BAR);
    }

    for (i = 0; i < nvariants; ++i) {
        if (verbose)
            athr_consume(at, 1);

        if (bgen_read_variant(bgen, variants + i))
            goto err;

        (*index)->start[i] = ftell(bgen->file);

        if (uint32_t_fread(bgen->file, &length, 4))
            goto err;

        if (fseek(bgen->file, length, SEEK_CUR)) {
            perror("Could not jump to a variants ");
            goto err;
        }

        if ((variants + i)->nalleles > (*index)->max_nalleles)
            (*index)->max_nalleles = (variants + i)->nalleles;
    }

    if (verbose)
        athr_finish(at);

    if (fclose(bgen->file)) {
        perror("Could not close the file.");
        goto err;
    }

    return variants;

err:

    if (bgen->file)
        fclose(bgen->file);

    if (*index != NULL)
        free(*index);

    if (variants != NULL)
        free(variants);

    if ((*index)->start != NULL)
        free((*index)->start);
    return NULL;
}

BGEN_API void bgen_free_variants_metadata(const struct bgen_file *bgen,
                                          struct bgen_var *variants) {
    size_t i, j;

    for (i = 0; i < (size_t)bgen->nvariants; ++i) {
        for (j = 0; j < (size_t)variants[i].nalleles; ++j) {
            free(variants[i].allele_ids[j].str);
        }
        free(variants[i].allele_ids);
        free(variants[i].id.str);
        free(variants[i].rsid.str);
        free(variants[i].chrom.str);
    }
    free(variants);
}

BGEN_API void bgen_free_index(struct bgen_vi *index) {
    free(index->filepath);
    free(index->start);
    free(index);
}

BGEN_API int bgen_max_nalleles(struct bgen_vi *vi) { return vi->max_nalleles; }

BGEN_API struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *vi, size_t index) {

    if (!vi) {
        error("bgen_vi pointer cannot be null.");
        return NULL;
    }
    struct bgen_vg *vg;
    FILE *fp = NULL;

    if (!(fp = fopen(vi->filepath, "rb"))) {
        perror("Could not open file ");
        return NULL;
    }

    vg = malloc(sizeof(struct bgen_vg));
    vg->variant_idx = index;
    vg->plo_miss = NULL;
    vg->chunk = NULL;

    if (fseek(fp, (long)vi->start[index], SEEK_SET)) {
        perror("Could not seek a variant ");
        free(vg);
        fclose(fp);
        return NULL;
    }

    if (vi->layout == 1) {
        bgen_read_probs_header_one(vi, vg, fp);
    } else if (vi->layout == 2) {
        bgen_read_probs_header_two(vi, vg, fp);
    } else {
        fprintf(stderr, "Unrecognized layout type.\n");
        free(vg);
        fclose(fp);
        return NULL;
    }

    fclose(fp);

    return vg;

err:
    FCLOSE(fp);
    return NULL;
}

BGEN_API void bgen_read_variant_genotype(struct bgen_vi *index, struct bgen_vg *vg,
                                         double *probabilities) {
    if (index->layout == 1) {
        bgen_read_probs_one(vg, probabilities);
    } else if (index->layout == 2) {
        bgen_read_probs_two(vg, probabilities);
    } else {
        fprintf(stderr, "Unrecognized layout type.\n");
    }
}

BGEN_API void bgen_close_variant_genotype(struct bgen_vi *index, struct bgen_vg *vg) {
    if (vg->plo_miss != NULL)
        free(vg->plo_miss);

    if (vg->chunk != NULL)
        free(vg->chunk);

    if (vg != NULL)
        free(vg);
}

BGEN_API int bgen_nalleles(const struct bgen_vg *vg) { return vg->nalleles; }

BGEN_API int bgen_missing(const struct bgen_vg *vg, size_t index) {
    return vg->plo_miss[index] >> 7;
}

BGEN_API int bgen_ploidy(const struct bgen_vg *vg, size_t index) {
    return vg->plo_miss[index] & 127;
}

BGEN_API int bgen_min_ploidy(const struct bgen_vg *vg) { return vg->min_ploidy; }

BGEN_API int bgen_max_ploidy(const struct bgen_vg *vg) { return vg->max_ploidy; }

BGEN_API int bgen_ncombs(const struct bgen_vg *vg) { return vg->ncombs; }

BGEN_API int bgen_phased(const struct bgen_vg *vg) { return vg->phased; }

struct bgen_cmf_next {
    struct bgen_file *bgen;
    uint32_t nvariants;
};

struct bgen_var *bgen_next_variant_metadata(uint64_t *genotype_offset, void *ctx) {

    struct bgen_var *variant = malloc(sizeof(struct bgen_var));
    struct bgen_cmf_next *self = ctx;
    uint32_t length;

    if (self->nvariants == 0)
        return NULL;

    if (bgen_read_variant(self->bgen, variant))
        goto err;

    *genotype_offset = ftell(self->bgen->file);

    if (fread(&length, 1, 4, self->bgen->file) < 4)
        goto err;

    if (fseek(self->bgen->file, length, SEEK_CUR)) {
        perror("Could not jump to the next variant ");
        goto err;
    }

    --(self->nvariants);
    return variant;
err:
    return NULL;
}

BGEN_API int bgen_create_metafile(struct bgen_file *bgen, const char *filepath,
                                  int verbose) {

    if (!bgen) {
        error("bgen_file pointer cannot be null.");
        return 1;
    }

    if (!filepath) {
        error("filepath pointer cannot be null.");
        return 1;
    }

    struct cmf *cmf = create_metafile(filepath, bgen_nvariants(bgen), 2);
    if (!cmf)
        return 1;

    struct bgen_cmf_next next_ctx;
    next_ctx.bgen = bgen;
    next_ctx.nvariants = bgen_nvariants(bgen);

    bopen_or_leave(bgen);

    if (fseek(bgen->file, bgen->variants_start, SEEK_SET)) {
        perror("Could not jump to variants start ");
        fclose(bgen->file);
        goto err;
    }

    if (write_metafile(cmf, &bgen_next_variant_metadata, &next_ctx, verbose))
        goto err;

    if (close_metafile(cmf))
        goto err;

    fclose(bgen->file);
    return 0;
err:
    FCLOSE(bgen->file);
    close_metafile(cmf);
    return 1;
}
