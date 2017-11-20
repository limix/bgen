#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgen/bgen.h"
#include "bgen/number.h"
#include "bgen/string.h"
#include "layout/one.h"
#include "layout/two.h"

#include "util/file.h"
#include "util/mem.h"
#include "variant_genotype.h"
#include "variants_index.h"

inti bgen_fread_string2(FILE *file, string *s);
inti bgen_fread_string4(FILE *file, string *s);

inti bgen_read_header(struct BGenFile *bgen) {
    uint32_t header_length;
    uint32_t nvariants;
    uint32_t nsamples;
    uint32_t magic_number;
    uint32_t flags;

    if (bgen_read(bgen->file, &header_length, 4) == FAIL)
        return FAIL;

    if (bgen_read(bgen->file, &nvariants, 4) == FAIL)
        return FAIL;

    if (bgen_read(bgen->file, &nsamples, 4) == FAIL)
        return FAIL;

    if (bgen_read(bgen->file, &magic_number, 4) == FAIL)
        return FAIL;

    fseek(bgen->file, header_length - 20, SEEK_CUR);

    if (bgen_read(bgen->file, &flags, 4))
        return FAIL;

    bgen->nvariants = nvariants;
    bgen->nsamples = nsamples;
    bgen->compression = flags & 3;
    bgen->layout = (flags & (15 << 2)) >> 2;
    bgen->sample_ids_presence = (flags & (1 << 31)) >> 31;

    return SUCCESS;
}

struct BGenFile *bgen_open(const byte *filepath) {
    uint32_t offset;
    struct BGenFile *bgen = malloc(sizeof(struct BGenFile));

    bgen->filepath = bgen_strdup(filepath);

    bgen->file = fopen(bgen->filepath, "rb");

    if (bgen->file == NULL) {
        fprintf(stderr, "Could not open %s", bgen->filepath);
        perror(NULL);
        goto err;
    }

    bgen->samples_start = -1;
    bgen->variants_start = -1;

    if (bgen_read(bgen->file, &offset, 4) == FAIL)
        goto err;

    bgen->variants_start = offset + 4;

    if (bgen_read_header(bgen) == FAIL)
        goto err;

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

void bgen_close(struct BGenFile *bgen) {
    if (bgen->filepath)
        free(bgen->filepath);

    if (bgen)
        free(bgen);
}

inti bgen_nsamples(struct BGenFile *bgen) { return bgen->nsamples; }

inti bgen_nvariants(struct BGenFile *bgen) { return bgen->nvariants; }

string *bgen_read_samples(struct BGenFile *bgen) {
    uint32_t length, nsamples;
    inti i;
    string *sample_ids;

    bgen->file = fopen(bgen->filepath, "rb");

    fseek(bgen->file, bgen->samples_start, SEEK_SET);

    if (bgen->sample_ids_presence == 0) {
        // bgen->variants_start = ftell(bgen->file);

        fclose(bgen->file);
        return NULL;
    }

    sample_ids = malloc(bgen->nsamples * sizeof(string));

    if (bgen_read(bgen->file, &length, 4))
        goto err;

    if (bgen_read(bgen->file, &nsamples, 4))
        goto err;

    for (i = 0; i < bgen->nsamples; ++i) {
        if (bgen_fread_string2(bgen->file, sample_ids + i) == FAIL)
            goto err;
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

void bgen_free_samples(const struct BGenFile *bgen, string *samples) {
    inti i;

    if (bgen->sample_ids_presence == 0)
        return;

    if (samples == NULL)
        return;

    for (i = 0; i < bgen->nsamples; ++i) {
        free(samples[i].str);
    }
    free(samples);
}

inti bgen_read_variant(struct BGenFile *bgen, struct BGVar *v) {
    inti i;
    uint32_t nsamples, position;
    uint16_t nalleles;

    if (bgen->layout == 1) {
        if (bgen_read(bgen->file, &nsamples, 4) == FAIL)
            return FAIL;
    }

    if (bgen_fread_string2(bgen->file, &v->id) == FAIL)
        return FAIL;

    if (bgen_fread_string2(bgen->file, &v->rsid) == FAIL)
        return FAIL;

    if (bgen_fread_string2(bgen->file, &v->chrom) == FAIL)
        return FAIL;

    if (bgen_read(bgen->file, &position, 4) == FAIL)
        return FAIL;

    v->position = position;

    if (bgen->layout == 1)
        nalleles = 2;
    else if (bgen_read(bgen->file, &nalleles, 2) == FAIL)
        return FAIL;

    v->nalleles = nalleles;

    v->allele_ids = malloc(nalleles * sizeof(string));

    for (i = 0; i < v->nalleles; ++i) {
        if (bgen_fread_string4(bgen->file, v->allele_ids + i) == FAIL)
            return FAIL;
    }

    return SUCCESS;
}

struct BGVar *bgen_read_variants(struct BGenFile *bgen, struct BGenVI **index) {
    struct BGVar *variants;
    uint32_t length;
    inti i, nvariants;

    variants = NULL;
    bgen->file = fopen(bgen->filepath, "rb");

    if (bgen->file == NULL) {
        fprintf(stderr, "Could not open %s", bgen->filepath);
        perror(NULL);
        return NULL;
    }

    if (fseek(bgen->file, bgen->variants_start, SEEK_SET)) {
        fprintf(stderr, "Could not jump to variants start");
        perror(NULL);
        goto err;
    }

    *index = bgen_new_index(bgen);

    nvariants = bgen->nvariants;
    variants = malloc(nvariants * sizeof(struct BGVar));

    for (i = 0; i < nvariants; ++i) {
        if (bgen_read_variant(bgen, variants + i) == FAIL)
            goto err;
        (*index)->start[i] = ftell(bgen->file);

        if (bgen_read(bgen->file, &length, 4) == FAIL)
            goto err;

        if (fseek(bgen->file, length, SEEK_CUR)) {
            fprintf(stderr, "Could not jump to a variants");
            perror(NULL);
            goto err;
        }
    }

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

void bgen_free_variants(const struct BGenFile *bgen, struct BGVar *variants) {
    inti i, j;

    for (i = 0; i < bgen->nvariants; ++i) {
        for (j = 0; j < variants[i].nalleles; ++j) {
            free(variants[i].allele_ids[j].str);
        }
        free(variants[i].allele_ids);
        free(variants[i].id.str);
        free(variants[i].rsid.str);
        free(variants[i].chrom.str);
    }
    free(variants);
}

void bgen_free_index(struct BGenVI *index) {
    free(index->filepath);
    free(index->start);
    free(index);
}

struct BGenVG *bgen_open_variant_genotype(struct BGenVI *index,
                                            inti variant_idx) {
    struct BGenVG *vg;
    FILE *file = fopen(index->filepath, "rb");

    if (file == NULL) {
        fprintf(stderr, "Could not open %s", index->filepath);
        perror(NULL);
        return NULL;
    }

    vg = malloc(sizeof(struct BGenVG));

    vg->variant_idx = variant_idx;
    vg->plo_miss = NULL;
    vg->chunk = NULL;
    if (fseek(file, index->start[variant_idx], SEEK_SET)) {
        fprintf(stderr, "Could not seek a variant");
        perror(NULL);
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

void bgen_read_variant_genotype(struct BGenVI *index, struct BGenVG *vg,
                                real *probabilities) {
    if (index->layout == 1) {
        bgen_read_probs_one(vg, probabilities);
    } else if (index->layout == 2) {
        bgen_read_probs_two(vg, probabilities);
    } else {
        fprintf(stderr, "Unrecognized layout type.\n");
    }
}

void bgen_close_variant_genotype(struct BGenVI *index, struct BGenVG *vg) {
    if (vg->plo_miss != NULL)
        free(vg->plo_miss);

    if (vg->chunk != NULL)
        free(vg->chunk);

    if (vg != NULL)
        free(vg);
}

inti bgen_nalleles(struct BGenVG *vg) { return vg->nalleles; }

inti bgen_ploidy(struct BGenVG *vg) { return vg->ploidy; }

inti bgen_ncombs(struct BGenVG *vg) { return vg->ncombs; }

inti bgen_sample_ids_presence(struct BGenFile *bgen) {
    return bgen->sample_ids_presence;
}
