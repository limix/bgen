#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "layout/two.h"
#include "variant_genotype.h"
#include "variants_index.h"

#include "bgen/bgen.h"
#include "util/bits.h"

#include "util/choose.h"
#include "util/file.h"
#include "util/mem.h"
#include "zip/zlib_wrapper.h"
#include "zip/zstd_wrapper.h"

inline static inti bgen_read_ploidy(byte ploidy_miss) {
    return ploidy_miss & 127;
}

inline static inti bgen_read_missingness(byte ploidy_miss) {
    return ploidy_miss >> 7;
}

inline static inti bit_sample_start(inti sample_idx, inti nbits, inti ncomb) {
    return sample_idx * (nbits * (ncomb - 1));
}

inline static inti bit_geno_start(inti geno_idx, inti nbits) {
    return geno_idx * nbits;
}

inline static int get_bit(const byte *mem, inti bit_idx) {
    inti bytes = bit_idx / 8;

    return GetBit(*(mem + bytes), bit_idx % 8);
}

void bgen_read_unphased_genotype(struct BGenVG *vg, real *p) {
    inti ncombs, ploidy, uip_sum, ui_prob;
    inti sample_start, geno_start, bit_idx;
    real denom = (((inti)1 << vg->nbits)) - 1;

    inti i, j, bi;

    for (j = 0; j < vg->nsamples; ++j) {
        ploidy = bgen_read_ploidy(vg->plo_miss[j]);

        ncombs = bgen_choose(vg->nalleles + ploidy - 1, vg->nalleles - 1);

        if (bgen_read_missingness(vg->plo_miss[j]) != 0) {
            for (i = 0; i < ncombs; ++i) {
                p[j * ncombs + i] = NAN;
            }
            continue;
        }

        uip_sum = 0;

        for (i = 0; i < ncombs - 1; ++i) {
            ui_prob = 0;

            for (bi = 0; bi < vg->nbits; ++bi) {
                sample_start = bit_sample_start(j, vg->nbits, ncombs);
                geno_start = bit_geno_start(i, vg->nbits);
                bit_idx = sample_start + geno_start + bi;

                if (get_bit(vg->current_chunk, bit_idx)) {
                    ui_prob |= ((inti)1 << bi);
                }
            }

            p[j * ncombs + i] = ui_prob / denom;
            uip_sum += ui_prob;
        }
        p[j * ncombs + ncombs - 1] = (denom - uip_sum) / denom;
    }
}

byte *bgen_uncompress_two(struct BGenVI *idx, FILE *file) {
    inti clength = 0, ulength = 0;
    byte *cchunk;
    byte *uchunk;

    if (bgen_read(file, &clength, 4) == FAIL)
        return NULL;

    clength -= 4;

    if (bgen_read(file, &ulength, 4) == FAIL)
        return NULL;

    cchunk = malloc(clength);

    if (bgen_read(file, cchunk, clength) == FAIL) {
        free(cchunk);
        return NULL;
    }

    uchunk = malloc(ulength);

    if (idx->compression == 1) {
        if (bgen_unzlib(cchunk, clength, &uchunk, &ulength) == FAIL)
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
    } else if (idx->compression == 2) {
        if (bgen_unzstd(cchunk, clength, &uchunk, &ulength) == FAIL)
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
    } else {
        fprintf(stderr, "Unrecognized compression method.");
        return NULL;
    }

    free(cchunk);

    return uchunk;
}

inti bgen_read_probs_header_two(struct BGenVI *idx, struct BGenVG *vg,
                                FILE *file) {
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t min_ploidy, max_ploidy, phased, nbits;
    uint8_t *plo_miss;

    byte *c;
    byte *chunk;
    inti i;

    if (idx->compression > 0) {
        if ((chunk = bgen_uncompress_two(idx, file)) == NULL)
            return FAIL;
        c = chunk;
        bgen_memcpy(&nsamples, &c, 4);
    } else {
        if (bgen_read(file, &nsamples, 4) == FAIL)
            return FAIL;

        chunk = malloc(6 * nsamples);

        if (bgen_read(file, chunk, 6 * nsamples) == FAIL)
            return FAIL;

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

void bgen_read_probs_two(struct BGenVG *vg, real *p) {
    bgen_read_unphased_genotype(vg, p);
}
