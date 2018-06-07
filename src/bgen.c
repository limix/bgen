#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgen.h"
#include "bgen_err.h"
#include "bgen_file.h"
#include "layout/one.h"
#include "layout/two.h"

#include "util/mem.h"
#include "variant_genotype.h"
#include "variants_index.h"

#include "util/string.h"

#include "athr.h"

int bgen_read_header(struct bgen_file *bgen) {
    uint32_t header_length;
    uint32_t nvariants;
    uint32_t nsamples;
    uint32_t magic_number;
    uint32_t flags;

    if (fread(&header_length, 1, 4, bgen->file) < 4) {
        fprintf(stderr, "Could not read the header length.\n");
        return 1;
    }

    if (fread(&nvariants, 1, 4, bgen->file) < 4) {
        fprintf(stderr, "Could not read the number of variants.\n");
        return 1;
    }

    if (fread(&nsamples, 1, 4, bgen->file) < 4) {
        fprintf(stderr, "Could not read the number of samples.\n");
        return 1;
    }

    if (fread(&magic_number, 1, 4, bgen->file) < 4) {
        fprintf(stderr, "Could not read the magic number.\n");
        return 1;
    }

    if (magic_number != 1852139362) {
        fprintf(stderr, "This is not a BGEN file: magic number mismatch.\n");
        return 1;
    }

    fseek(bgen->file, header_length - 20, SEEK_CUR);

    if (fread(&flags, 1, 4, bgen->file) < 4) {
        fprintf(stderr, "Could not read the bgen flags.\n");
        return 1;
    }

    bgen->nvariants = nvariants;
    bgen->nsamples = nsamples;
    bgen->compression = flags & 3;
    bgen->layout = (flags & (15 << 2)) >> 2;
    bgen->sample_ids_presence = (flags & (1 << 31)) >> 31;

    return 0;
}

/**
 * filepath
 *  File path to the bgen file.
 */
struct bgen_file *bgen_open(const char *filepath) {
    uint32_t offset;
    struct bgen_file *bgen;

    bgen = malloc(sizeof(struct bgen_file));
    bgen->samples_start = -1;
    bgen->variants_start = -1;
    bgen->file = NULL;

    bgen->filepath = bgen_strdup(filepath);

    if ((bgen->file = fopen(bgen->filepath, "rb")) == NULL) {
        perror("Could not open bgen file ");
        goto err;
    }

    if (fread(&offset, 1, 4, bgen->file) < 4) {
        perror("Could not read offset ");
        goto err;
    }

    bgen->variants_start = offset + 4;

    if (bgen_read_header(bgen)) {
        fprintf(stderr, "Could not read bgen header.");
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
        perror("Could not close file");
        goto err;
    }

    return bgen;

err:

    if (bgen->file)
        fclose(bgen->file);

    if (bgen->filepath)
        free(bgen->filepath);

    if (bgen != NULL)
        free(bgen);

    return NULL;
}

void bgen_close(struct bgen_file *bgen) {
    if (bgen->filepath)
        free(bgen->filepath);

    if (bgen)
        free(bgen);
}

int bgen_nsamples(const struct bgen_file *bgen) { return bgen->nsamples; }

int bgen_nvariants(const struct bgen_file *bgen) { return bgen->nvariants; }

int bgen_sample_ids_presence(const struct bgen_file *bgen) {
    return bgen->sample_ids_presence;
}

struct bgen_string *bgen_read_samples(struct bgen_file *bgen, int verbose) {
    uint32_t length, nsamples;
    size_t i;
    struct bgen_string *sample_ids;
    struct athr *at = NULL;

    bgen->file = fopen(bgen->filepath, "rb");

    fseek(bgen->file, bgen->samples_start, SEEK_SET);

    if (bgen->sample_ids_presence == 0) {
        fclose(bgen->file);
        return NULL;
    }

    sample_ids = malloc(bgen->nsamples * sizeof(struct bgen_string));

    if (fread(&length, 1, 4, bgen->file) < 4)
        goto err;

    if (fread(&nsamples, 1, 4, bgen->file) < 4)
        goto err;

    if (verbose) {
        at = athr_create(bgen->nsamples, "Reading samples", ATHR_BAR);
    }

    for (i = 0; i < (size_t)bgen->nsamples; ++i) {
        if (verbose) {
            athr_consume(at, 1);
        }
        if (fread_string(bgen->file, sample_ids + i, 2))
            goto err;
    }

    if (verbose) {
        athr_finish(at);
    }

    bgen->variants_start = ftell(bgen->file);

    if (fclose(bgen->file)) {
        perror("Could not close the file.");
        goto err;
    }

    return sample_ids;

err:

    if (bgen->file != NULL)
        fclose(bgen->file);

    if (sample_ids != NULL)
        free(sample_ids);
    return NULL;
}

void bgen_free_samples(const struct bgen_file *bgen,
                       struct bgen_string *samples) {
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
    size_t i;
    uint32_t nsamples, position;
    uint16_t nalleles;

    if (bgen->layout == 1) {
        if (fread(&nsamples, 1, 4, bgen->file) < 4)
            return 1;
    }

    if (fread_string(bgen->file, &v->id, 2))
        return 1;

    if (fread_string(bgen->file, &v->rsid, 2))
        return 1;

    if (fread_string(bgen->file, &v->chrom, 2))
        return 1;

    if (fread(&position, 1, 4, bgen->file) < 4)
        return 1;

    v->position = position;

    if (bgen->layout == 1)
        nalleles = 2;
    else if (fread(&nalleles, 1, 2, bgen->file) < 2)
        return 1;

    v->nalleles = nalleles;

    v->allele_ids = malloc(nalleles * sizeof(struct bgen_string));

    for (i = 0; i < (size_t)v->nalleles; ++i) {
        if (fread_string(bgen->file, v->allele_ids + i, 4))
            return 1;
    }

    return 0;
}

struct bgen_var *bgen_read_variants_metadata(struct bgen_file *bgen,
                                             struct bgen_vi **index,
                                             int verbose) {
    struct bgen_var *variants;
    uint32_t length;
    size_t i, nvariants;
    struct athr *at = NULL;

    variants = NULL;
    if ((bgen->file = fopen(bgen->filepath, "rb")) == NULL) {
        perror("Could not open ");
        goto err;
    }

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

        if (fread(&length, 1, 4, bgen->file) < 4)
            goto err;

        if (fseek(bgen->file, length, SEEK_CUR)) {
            perror("Could not jump to a variants ");
            goto err;
        }
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

void bgen_free_variants_metadata(const struct bgen_file *bgen,
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

void bgen_free_index(struct bgen_vi *index) {
    free(index->filepath);
    free(index->start);
    free(index);
}

struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *index,
                                           size_t variant_idx) {
    struct bgen_vg *vg;
    FILE *file;

    if ((file = fopen(index->filepath, "rb")) == NULL) {
        perror("Could not open file ");
        return NULL;
    }

    vg = malloc(sizeof(struct bgen_vg));
    vg->variant_idx = variant_idx;
    vg->plo_miss = NULL;
    vg->chunk = NULL;

    if (fseek(file, (long)index->start[variant_idx], SEEK_SET)) {
        perror("Could not seek a variant ");
        return NULL;
    }

    if (index->layout == 1) {
        bgen_read_probs_header_one(index, vg, file);
    } else if (index->layout == 2) {
        bgen_read_probs_header_two(index, vg, file);
    } else {
        fprintf(stderr, "Unrecognized layout type.\n");
        return NULL;
    }

    fclose(file);

    return vg;
}

void bgen_read_variant_genotype(struct bgen_vi *index, struct bgen_vg *vg,
                                double *probabilities) {
    if (index->layout == 1) {
        bgen_read_probs_one(vg, probabilities);
    } else if (index->layout == 2) {
        bgen_read_probs_two(vg, probabilities);
    } else {
        fprintf(stderr, "Unrecognized layout type.\n");
    }
}

void bgen_close_variant_genotype(struct bgen_vi *index, struct bgen_vg *vg) {
    if (vg->plo_miss != NULL)
        free(vg->plo_miss);

    if (vg->chunk != NULL)
        free(vg->chunk);

    if (vg != NULL)
        free(vg);
}

int bgen_nalleles(const struct bgen_vg *vg) { return vg->nalleles; }

int bgen_ploidy(const struct bgen_vg *vg) { return vg->ploidy; }

int bgen_ncombs(const struct bgen_vg *vg) { return vg->ncombs; }

int bgen_phased(const struct bgen_vg *vg) { return vg->phased; }