#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "layout/one.h"
#include "variant_genotype.h"
#include "variants_index.h"

#include "bgen/bgen.h"
#include "util/bits.h"

#include "util/choose.h"
#include "util/file.h"
#include "util/mem.h"
#include "zip/zlib_wrapper.h"
#include "zip/zstd_wrapper.h"

void bgen_read_unphased_one(struct BGenVG *vg, real *probabilities) {
    uint16_t ui_prob;
    inti ui_prob_sum;

    real denom = 32768;

    inti i, j;

    byte *chunk = vg->current_chunk;

    for (j = 0; j < vg->nsamples; ++j) {
        ui_prob_sum = 0;

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

void bgen_read_probs_one(struct BGenVG *vg, real *p) {
    bgen_read_unphased_one(vg, p);
}

byte *bgen_uncompress_layout1(struct BGenVI *index, FILE *file) {
    inti clength = 0, ulength = 0;
    byte *cchunk;
    byte *uchunk;

    if (bgen_read(file, &clength, 4) == FAIL)
        return NULL;

    cchunk = malloc(clength);

    if (bgen_read(file, cchunk, clength) == FAIL) {
        free(cchunk);
        return NULL;
    }

    if (index->compression != 1) {
        free(cchunk);
        fprintf(stderr, "Compression flag should be 1; not %lld.\n",
                index->compression);
        return NULL;
    }

    ulength = 10 * clength;
    uchunk = malloc(ulength);

    bgen_unzlib_chunked(cchunk, clength, &uchunk, &ulength);

    free(cchunk);

    return uchunk;
}

inti bgen_read_probs_header_one(struct BGenVI *index, struct BGenVG *vg,
                                FILE *file) {
    byte *c;
    byte *chunk;

    if (index->compression > 0) {
        chunk = bgen_uncompress_layout1(index, file);
        c = chunk;
    } else {
        chunk = malloc(6 * index->nsamples);

        if (bgen_read(file, chunk, 6 * index->nsamples) == FAIL)
            return FAIL;

        c = chunk;
    }

    vg->nsamples = index->nsamples;
    vg->nalleles = 2;
    vg->ncombs = 3;
    vg->ploidy = 2;
    vg->chunk = chunk;
    vg->current_chunk = c;

    return EXIT_SUCCESS;
}
