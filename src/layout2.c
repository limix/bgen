#include "layout2.h"
#include "bgen/bgen.h"
#include "file.h"
#include "geno.h"
#include "index.h"
#include "math.h"
#include "mem.h"
#include "zip/zlib.h"
#include "zip/zstd.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define BIT(var, bit) ((var & (1 << bit)) != 0)

static unsigned choose(unsigned n, unsigned k);
static void     read_phased_genotype(struct bgen_genotype* vg, double* p);
static void     read_unphased_genotype(struct bgen_genotype* vg, double* p);
static char*    _uncompress(struct bgen_vi* idx, FILE* file);

inline static int read_ploidy(char ploidy_miss) { return ploidy_miss & 127; }

inline static int read_missingness(char ploidy_miss) { return ploidy_miss >> 7; }

inline static int get_bit(const char* mem, int bit_idx)
{
    int bytes = bit_idx / 8;

    return BIT(*(mem + bytes), bit_idx % 8);
}

inline static void set_array_nan(double* p, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        p[i] = NAN;
}

int bgen_layout2_read_header(struct bgen_vi* idx, struct bgen_genotype* vg, FILE* file)
{
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t  min_ploidy, max_ploidy, phased, nbits;
    uint8_t* plo_miss;

    char*  c;
    char*  chunk;
    size_t i;

    if (idx->compression > 0) {
        if ((chunk = _uncompress(idx, file)) == NULL)
            return 1;
        c = chunk;
        memcpy_walk(&nsamples, &c, 4);
    } else {
        if (fread(&nsamples, 1, 4, file) < 4)
            return 1;

        chunk = malloc(6 * nsamples);

        if (fread(chunk, 1, 6 * nsamples, file) < 6 * nsamples) {
            free(chunk);
            return 1;
        }

        c = chunk;
    }

    memcpy_walk(&nalleles, &c, 2);
    memcpy_walk(&min_ploidy, &c, 1);
    memcpy_walk(&max_ploidy, &c, 1);
    vg->min_ploidy = min_ploidy;
    vg->max_ploidy = max_ploidy;

    plo_miss = malloc(nsamples * sizeof(uint8_t));

    for (i = 0; i < nsamples; ++i) {
        plo_miss[i] = c[i];
    }
    c += nsamples;

    memcpy_walk(&phased, &c, 1);
    memcpy_walk(&nbits, &c, 1);

    vg->nsamples = nsamples;
    vg->nalleles = nalleles;
    vg->phased = phased;
    vg->nbits = nbits;
    vg->plo_miss = plo_miss;

    if (phased)
        vg->ncombs = nalleles * vg->max_ploidy;
    else
        vg->ncombs = choose(nalleles + vg->max_ploidy - 1, nalleles - 1);
    vg->chunk = chunk;
    vg->current_chunk = c;

    return EXIT_SUCCESS;
}

void bgen_layout2_read_genotype(struct bgen_genotype* vg, double* p)
{
    if (vg->phased)
        read_phased_genotype(vg, p);
    else
        read_unphased_genotype(vg, p);
}

static void read_phased_genotype(struct bgen_genotype* vg, double* p)
{
    int      nalleles, nbits, max_ploidy;
    uint64_t uip_sum, ui_prob;
    uint64_t sample_start, haplo_start, allele_start;
    double   denom;

    nbits = vg->nbits;
    nalleles = vg->nalleles;
    max_ploidy = vg->max_ploidy;
    size_t i, ii, j, bi, offset;
    denom = (double)((((uint64_t)1 << nbits)) - 1);
    double* pend;

    sample_start = 0;
    for (j = 0; j < vg->nsamples; ++j) {
        int ploidy = read_ploidy(vg->plo_miss[j]);
        pend = p + max_ploidy * nalleles;

        if (read_missingness(vg->plo_miss[j]) != 0) {
            set_array_nan(p, (size_t)(pend - p));
            p = pend;
            sample_start += nbits * ploidy * (nalleles - 1);
            continue;
        }

        haplo_start = 0;
        for (i = 0; i < (size_t)ploidy; ++i) {

            uip_sum = 0;
            allele_start = 0;
            for (ii = 0; ii < (size_t)(nalleles - 1); ++ii) {

                ui_prob = 0;
                offset = sample_start + haplo_start + allele_start;

                for (bi = 0; bi < (size_t)nbits; ++bi) {

                    if (get_bit(vg->current_chunk, (int)(bi + offset))) {
                        ui_prob |= ((uint64_t)1 << bi);
                    }
                }

                *p = ui_prob / denom;
                ++p;
                uip_sum += ui_prob;
                allele_start += nbits;
            }
            *p = (denom - uip_sum) / denom;
            ++p;
            haplo_start += nbits * (nalleles - 1);
        }
        sample_start += nbits * ploidy * (nalleles - 1);
        set_array_nan(p, (size_t)(pend - p));
        p = pend;
    }
}

static void read_unphased_genotype(struct bgen_genotype* vg, double* p)
{
    int      nalleles, nbits, max_ploidy, max_ncombs;
    uint64_t uip_sum, ui_prob;
    uint64_t sample_start, geno_start;
    double   denom;

    nbits = vg->nbits;
    nalleles = vg->nalleles;
    max_ploidy = vg->max_ploidy;

    size_t i, j, bi, offset;
    denom = (double)((((uint64_t)1 << nbits)) - 1);
    double* pend;

    max_ncombs = choose(nalleles + max_ploidy - 1, nalleles - 1);

    sample_start = 0;
    for (j = 0; j < vg->nsamples; ++j) {
        pend = p + max_ncombs;
        int ploidy = read_ploidy(vg->plo_miss[j]);

        int ncombs = choose(nalleles + ploidy - 1, nalleles - 1);

        if (read_missingness(vg->plo_miss[j]) != 0) {
            set_array_nan(p, (size_t)(pend - p));
            p = pend;
            sample_start += nbits * (ncombs - 1);
            continue;
        }

        uip_sum = 0;

        geno_start = 0;
        for (i = 0; i < (size_t)(ncombs - 1); ++i) {

            ui_prob = 0;
            offset = sample_start + geno_start;

            for (bi = 0; bi < (size_t)nbits; ++bi) {

                if (get_bit(vg->current_chunk, (int)(bi + offset))) {
                    ui_prob |= ((uint64_t)1 << bi);
                }
            }
            *p = ui_prob / denom;
            ++p;
            uip_sum += ui_prob;
            geno_start += nbits;
        }
        *p = (denom - uip_sum) / denom;
        ++p;
        sample_start += nbits * (ncombs - 1);
        set_array_nan(p, (size_t)(pend - p));
        p = pend;
    }
}

static char* _uncompress(struct bgen_vi* idx, FILE* file)
{
    size_t clength, ulength;
    char*  cchunk;
    char*  uchunk;
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
        if (bgen_unzlib(cchunk, clength, &uchunk, &ulength)) {
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
            goto err;
        }
    } else if (idx->compression == 2) {
        if (bgen_unzstd(cchunk, clength, (void**)&uchunk, &ulength)) {
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
            goto err;
        }
    } else {
        fprintf(stderr, "Unrecognized compression method.");
        goto err;
    }

    free(cchunk);

    return uchunk;
err:
    fflush(stderr);

    free(cchunk);
    free(uchunk);
    return NULL;
}

/* Number of ways to choose `k` elements from a total of `n`. */
static unsigned choose(unsigned n, unsigned k)
{
    unsigned ans = 1;

    k = k > n - k ? n - k : k;
    unsigned j = 1;

    for (; j <= k; j++, n--) {
        if (n % j == 0) {
            ans *= n / j;
        } else if (ans % j == 0) {
            ans = ans / j * n;
        } else {
            ans = (ans * n) / j;
        }
    }
    return ans;
}
