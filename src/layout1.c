#include "layout1.h"
#include "bgen/bgen.h"
#include "file.h"
#include "genotype.h"
#include "mem.h"
#include "zip/zlib.h"
#include "zip/zstd.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

static void  read_unphased(struct bgen_genotype* vg, double* probabilities);
static char* _uncompress(struct bgen_file* bgen_file);

int bgen_layout1_read_header(struct bgen_file* bgen_file, struct bgen_genotype* genotype)
{
    char* c;
    char* chunk;

    if (bgen_file_compression(bgen_file) > 0) {
        chunk = _uncompress(bgen_file);
        c = chunk;
    } else {
        chunk = malloc(6 * bgen_file_nsamples(bgen_file));

        if (fread(chunk, 1, 6 * bgen_file_nsamples(bgen_file), bgen_file_stream(bgen_file)) <
            6 * bgen_file_nsamples(bgen_file)) {
            free(chunk);
            return 1;
        }

        c = chunk;
    }

    genotype->nsamples = bgen_file_nsamples(bgen_file);
    genotype->nalleles = 2;
    genotype->ncombs = 3;
    genotype->min_ploidy = 2;
    genotype->max_ploidy = 2;
    genotype->chunk = chunk;
    genotype->current_chunk = c;

    return 0;
}

void bgen_layout1_read_genotype(struct bgen_genotype* genotype, double* probabilities)
{
    read_unphased(genotype, probabilities);
}

static void read_unphased(struct bgen_genotype* genotype, double* probabilities)
{
    uint16_t ui_prob;

    double denom = 32768;

    size_t i, j;

    char const* restrict chunk = genotype->current_chunk;

    for (j = 0; j < genotype->nsamples; ++j) {
        unsigned int ui_prob_sum = 0;

        for (i = 0; i < 3; ++i) {
            bgen_memfread(&ui_prob, &chunk, 2);
            probabilities[j * 3 + i] = ui_prob / denom;
            ui_prob_sum += ui_prob;
        }

        if (ui_prob_sum == 0) {
            for (i = 0; i < 3; ++i)
                probabilities[j * 3 + i] = NAN;
        }
    }
}

static char* _uncompress(struct bgen_file* bgen_file)
{
    size_t clength, ulength;
    char * cchunk, *uchunk;

    clength = 0;
    ulength = 0;

    if (fread(&clength, 1, 4, bgen_file_stream(bgen_file)) < 4)
        return NULL;

    cchunk = malloc(clength);

    if (fread(cchunk, 1, clength, bgen_file_stream(bgen_file)) < clength) {
        free(cchunk);
        return NULL;
    }

    if (bgen_file_compression(bgen_file) != 1) {
        free(cchunk);
        fprintf(stderr, "Compression flag should be 1; not %u.\n",
                bgen_file_compression(bgen_file));
        return NULL;
    }

    ulength = 10 * clength;
    uchunk = malloc(ulength);

    bgen_unzlib_chunked(cchunk, clength, &uchunk, &ulength);

    free(cchunk);

    return uchunk;
}
