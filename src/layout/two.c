#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "layout/two.h"
#include "variant_genotype.h"
#include "variants_index.h"

#include "bgen/bgen.h"
#include "util/bits.h"

#include "bgen_file.h"
#include "util/choose.h"
#include "util/mem.h"
#include "zip/zlib_wrapper.h"
#include "zip/zstd_wrapper.h"

inline static int bgen_read_ploidy(char ploidy_miss) {
    return ploidy_miss & 127;
}

inline static int bgen_read_missingness(char ploidy_miss) {
    return ploidy_miss >> 7;
}

inline static int bit_sample_start(int sample_idx, int nbits, int ncomb) {
    return sample_idx * (nbits * (ncomb - 1));
}

inline static int bit_geno_start(int geno_idx, int nbits) {
    return geno_idx * nbits;
}

inline static int get_bit(const char *mem, int bit_idx) {
    int bytes = bit_idx / 8;

    return GetBit(*(mem + bytes), bit_idx % 8);
}

void bgen_read_unphased_genotype(struct BGenVG *vg, double *p) {
    int ncombs, ploidy;
    uint64_t uip_sum, ui_prob;
    uint64_t sample_start, geno_start, bit_idx;
    double denom;

    size_t i, j, bi;
    denom = (double) ((((uint64_t)1 << vg->nbits)) - 1);

    for (j = 0; j < vg->nsamples; ++j) {
        ploidy = bgen_read_ploidy(vg->plo_miss[j]);

        ncombs = bgen_choose(vg->nalleles + ploidy - 1, vg->nalleles - 1);

        if (bgen_read_missingness(vg->plo_miss[j]) != 0) {
            for (i = 0; i < (size_t) ncombs; ++i) {
                p[j * ncombs + i] = NAN;
            }
            continue;
        }

        uip_sum = 0;

        for (i = 0; i < (size_t) (ncombs - 1); ++i) {
            ui_prob = 0;

            for (bi = 0; bi < vg->nbits; ++bi) {
                sample_start = bit_sample_start(j, vg->nbits, ncombs);
                geno_start = bit_geno_start(i, vg->nbits);
                bit_idx = sample_start + geno_start + bi;

                if (get_bit(vg->current_chunk, (int) bit_idx)) {
                    ui_prob |= ((uint64_t)1 << bi);
                }
            }

            p[j * ncombs + i] = ui_prob / denom;
            uip_sum += ui_prob;
        }
        p[j * ncombs + ncombs - 1] = (denom - uip_sum) / denom;
    }
}

char *bgen_uncompress_two(struct BGenVI *idx, FILE *file) {
    size_t clength, ulength;
    char *cchunk;
    char *uchunk;
    clength = 0;
    ulength = 0;

    if (fread(&clength, 1, 4, file) < 4)
        return NULL;

    clength -= 4;

    if (fread(&ulength, 1, 4, file) < 4)
        return NULL;

    cchunk = malloc(clength);

    if (fread(cchunk, 1, clength, file) < clength) {
        free(cchunk);
        return NULL;
    }

    uchunk = malloc(ulength);

    if (idx->compression == 1) {
        if (bgen_unzlib(cchunk, clength, &uchunk, &ulength))
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
    } else if (idx->compression == 2) {
        if (bgen_unzstd(cchunk, clength, (void**) &uchunk, &ulength))
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
    } else {
        fprintf(stderr, "Unrecognized compression method.");
        return NULL;
    }

    free(cchunk);

    return uchunk;
}

int bgen_read_probs_header_two(struct BGenVI *idx, struct BGenVG *vg,
                               FILE *file) {
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t min_ploidy, max_ploidy, phased, nbits;
    uint8_t *plo_miss;

    char *c;
    char *chunk;
    size_t i;

    if (idx->compression > 0) {
        if ((chunk = bgen_uncompress_two(idx, file)) == NULL)
            return 1;
        c = chunk;
        bgen_memcpy(&nsamples, &c, 4);
    } else {
        if (fread(&nsamples, 1, 4, file) < 4)
            return 1;

        chunk = malloc(6 * nsamples);

        if (fread(chunk, 1, 6 * nsamples, file) < 6 * nsamples)
            return 1;

        c = chunk;
    }

    bgen_memcpy(&nalleles, &c, 2);
    bgen_memcpy(&min_ploidy, &c, 1);
    bgen_memcpy(&max_ploidy, &c, 1);
    vg->ploidy = max_ploidy;

    plo_miss = malloc(nsamples * sizeof(uint8_t));

    for (i = 0; i < nsamples; ++i) {
        plo_miss[i] = c[i];
    }
    c += nsamples;

    bgen_memcpy(&phased, &c, 1);
    bgen_memcpy(&nbits, &c, 1);

    assert(phased == 0);

    vg->nsamples = nsamples;
    vg->nalleles = nalleles;
    vg->nbits = nbits;
    vg->plo_miss = plo_miss;
    vg->ncombs = bgen_choose(nalleles + vg->ploidy - 1, nalleles - 1);
    vg->chunk = chunk;
    vg->current_chunk = c;

    return EXIT_SUCCESS;
}

void bgen_read_probs_two(struct BGenVG *vg, double *p) {
    bgen_read_unphased_genotype(vg, p);
}
