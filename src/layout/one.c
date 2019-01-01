#include "layout/one.h"
#include "bgen.h"
#include "choose.h"
#include "depr/variants_index.h"
#include "file.h"
#include "geno.h"
#include "mem.h"
#include "zip/zlib_wrapper.h"
#include "zip/zstd_wrapper.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

void bgen_read_unphased_one(struct bgen_vg *vg, double *probabilities)
{
    uint16_t ui_prob;

    double denom = 32768;

    size_t i, j;

    char *chunk = vg->current_chunk;

    for (j = 0; j < vg->nsamples; ++j) {
        unsigned int ui_prob_sum = 0;

        for (i = 0; i < 3; ++i) {
            bgen_memcpy(&ui_prob, &chunk, 2);
            probabilities[j * 3 + i] = ui_prob / denom;
            ui_prob_sum += ui_prob;
        }

        if (ui_prob_sum == 0) {
            for (i = 0; i < 3; ++i)
                probabilities[j * 3 + i] = NAN;
        }
    }
}

void bgen_read_probs_one(struct bgen_vg *vg, double *p)
{
    bgen_read_unphased_one(vg, p);
}

char *bgen_uncompress_layout1(struct bgen_vi *index, FILE *file)
{
    size_t clength, ulength;
    char *cchunk, *uchunk;

    clength = 0;
    ulength = 0;

    if (fread(&clength, 1, 4, file) < 4)
        return NULL;

    cchunk = malloc(clength);

    if (fread(cchunk, 1, clength, file) < clength) {
        free(cchunk);
        return NULL;
    }

    if (index->compression != 1) {
        free(cchunk);
        fprintf(stderr, "Compression flag should be 1; not %u.\n", index->compression);
        return NULL;
    }

    ulength = 10 * clength;
    uchunk = malloc(ulength);

    bgen_unzlib_chunked(cchunk, clength, &uchunk, &ulength);

    free(cchunk);

    return uchunk;
}

int bgen_read_probs_header_one(struct bgen_vi *index, struct bgen_vg *vg, FILE *file)
{
    char *c;
    char *chunk;

    if (index->compression > 0) {
        chunk = bgen_uncompress_layout1(index, file);
        c = chunk;
    } else {
        chunk = malloc(6 * index->nsamples);

        if (fread(chunk, 1, 6 * index->nsamples, file) < 6 * index->nsamples) {
            free(chunk);
            return 1;
        }

        c = chunk;
    }

    vg->nsamples = index->nsamples;
    vg->nalleles = 2;
    vg->ncombs = 3;
    vg->min_ploidy = 2;
    vg->max_ploidy = 2;
    vg->chunk = chunk;
    vg->current_chunk = c;

    return 0;
}
