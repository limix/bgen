#include "layout1.h"
#include "bgen/bgen.h"
#include "file.h"
#include "geno.h"
#include "index.h"
#include "mem.h"
#include "zip/zlib.h"
#include "zip/zstd.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

static void  read_unphased(struct bgen_vg* vg, double* probabilities);
static char* _uncompress(struct bgen_vi* index, FILE* file);

int bgen_layout1_read_header(struct bgen_vi* index, struct bgen_vg* vg, FILE* file)
{
    char* c;
    char* chunk;

    if (index->compression > 0) {
        chunk = _uncompress(index, file);
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

void bgen_layout1_read_genotype(struct bgen_vg* vg, double* p) { read_unphased(vg, p); }

static void read_unphased(struct bgen_vg* vg, double* probabilities)
{
    uint16_t ui_prob;

    double denom = 32768;

    size_t i, j;

    char* chunk = vg->current_chunk;

    for (j = 0; j < vg->nsamples; ++j) {
        unsigned int ui_prob_sum = 0;

        for (i = 0; i < 3; ++i) {
            memcpy_walk(&ui_prob, &chunk, 2);
            probabilities[j * 3 + i] = ui_prob / denom;
            ui_prob_sum += ui_prob;
        }

        if (ui_prob_sum == 0) {
            for (i = 0; i < 3; ++i)
                probabilities[j * 3 + i] = NAN;
        }
    }
}

static char* _uncompress(struct bgen_vi* index, FILE* file)
{
    size_t clength, ulength;
    char * cchunk, *uchunk;

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
