#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "layout/two.h"
#include "variant_genotype.h"
#include "variants_index.h"

#include "bgen.h"
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

inline static int phased_bit_sample_start(int sample_idx, int nbits, int ploidy,
                                          int nalleles) {
    return sample_idx * nbits * ploidy * (nalleles - 1);
}

inline static int unphased_bit_sample_start(int sample_idx, int nbits,
                                            int ncomb) {
    return sample_idx * nbits * (ncomb - 1);
}

inline static int phased_bit_haplo_start(int haplo_idx, int nbits,
                                         int nalleles) {
    return haplo_idx * nbits * (nalleles - 1);
}

inline static int unphased_bit_geno_start(int geno_idx, int nbits) {
    return geno_idx * nbits;
}

inline static int get_bit(const char *mem, int bit_idx) {
    int bytes = bit_idx / 8;

    return GetBit(*(mem + bytes), bit_idx % 8);
}

inline static void set_array_nan(double *p, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        p[i] = NAN;
    }
}

void bgen_read_phased_genotype(struct bgen_vg *vg, double *p) {
    int nalleles, ploidy, nbits;
    uint64_t uip_sum, ui_prob;
    uint64_t sample_start, haplo_start, allele_start;
    double denom;

    nbits = vg->nbits;
    nalleles = vg->nalleles;
    size_t i, ii, j, bi, offset;
    denom = (double)((((uint64_t)1 << nbits)) - 1);

    for (j = 0; j < vg->nsamples; ++j) {
        ploidy = bgen_read_ploidy(vg->plo_miss[j]);

        if (bgen_read_missingness(vg->plo_miss[j]) != 0) {
            set_array_nan(p + j * ploidy * nalleles,
                          (size_t)(ploidy * nalleles));
            continue;
        }

        sample_start = j * vg->nbits * ploidy * (nalleles - 1);
        for (i = 0; i < (size_t)ploidy; ++i) {

            uip_sum = 0;
            haplo_start = i * vg->nbits * (nalleles - 1);

            for (ii = 0; ii < (size_t)(nalleles - 1); ++ii) {

                ui_prob = 0;
                allele_start = ii * vg->nbits;
                offset = sample_start + haplo_start + allele_start;

                for (bi = 0; bi < nbits; ++bi) {

                    if (get_bit(vg->current_chunk, (int)(bi + offset))) {
                        ui_prob |= ((uint64_t)1 << bi);
                    }
                }

                p[j * ploidy * nalleles + i * nalleles + ii] = ui_prob / denom;
                uip_sum += ui_prob;
            }
            p[j * ploidy * nalleles + i * nalleles + nalleles - 1] =
                (denom - uip_sum) / denom;
        }
    }
}

void bgen_read_unphased_genotype(struct bgen_vg *vg, double *p) {
    int ncombs, ploidy;
    uint64_t uip_sum, ui_prob;
    uint64_t sample_start, geno_start;
    double denom;

    size_t i, j, bi, offset;
    denom = (double)((((uint64_t)1 << vg->nbits)) - 1);

    for (j = 0; j < vg->nsamples; ++j) {
        ploidy = bgen_read_ploidy(vg->plo_miss[j]);

        ncombs = bgen_choose(vg->nalleles + ploidy - 1, vg->nalleles - 1);

        if (bgen_read_missingness(vg->plo_miss[j]) != 0) {
            set_array_nan(p + j * ncombs, (size_t)ncombs);
            continue;
        }

        uip_sum = 0;

        sample_start = j * vg->nbits * (ncombs - 1);
        for (i = 0; i < (size_t)(ncombs - 1); ++i) {

            ui_prob = 0;
            geno_start = i * vg->nbits;
            offset = sample_start + geno_start;

            for (bi = 0; bi < vg->nbits; ++bi) {

                if (get_bit(vg->current_chunk, (int)(bi + offset))) {
                    ui_prob |= ((uint64_t)1 << bi);
                }
            }

            p[j * ncombs + i] = ui_prob / denom;
            uip_sum += ui_prob;
        }
        p[j * ncombs + ncombs - 1] = (denom - uip_sum) / denom;
    }
}

char *bgen_uncompress_two(struct bgen_vi *idx, FILE *file) {
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
        if (bgen_unzstd(cchunk, clength, (void **)&uchunk, &ulength))
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
    } else {
        fprintf(stderr, "Unrecognized compression method.");
        return NULL;
    }

    free(cchunk);

    return uchunk;
}

int bgen_read_probs_header_two(struct bgen_vi *idx, struct bgen_vg *vg,
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

    vg->nsamples = nsamples;
    vg->nalleles = nalleles;
    vg->phased = phased;
    vg->nbits = nbits;
    vg->plo_miss = plo_miss;
    if (phased)
        vg->ncombs = nalleles * vg->ploidy;
    else
        vg->ncombs = bgen_choose(nalleles + vg->ploidy - 1, nalleles - 1);
    vg->chunk = chunk;
    vg->current_chunk = c;

    return EXIT_SUCCESS;
}

void bgen_read_probs_two(struct bgen_vg *vg, double *p) {
    if (vg->phased)
        bgen_read_phased_genotype(vg, p);
    else
        bgen_read_unphased_genotype(vg, p);
}
