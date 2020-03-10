#include "layout2.h"
#include "bmath.h"
#include "genotype.h"
#include "mem.h"
#include "zip/zlib.h"
#include "zip/zstd.h"
#include <math.h>

#define BIT(var, bit) ((var & (1 << bit)) != 0)

static void  read_phased_genotype(struct bgen_genotype* genotype, double* probabilities);
static void  read_unphased_genotype(struct bgen_genotype* genotype, double* probabilities);
static char* _uncompress(struct bgen_vi* idx, FILE* file);

inline static uint8_t read_ploidy(uint8_t ploidy_miss) { return ploidy_miss & 127; }

inline static int read_missingness(uint8_t ploidy_miss) { return ploidy_miss >> 7; }

inline static int get_bit(char const* mem, uint64_t bit_idx)
{
    uint64_t bytes = bit_idx / 8;
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

    char* c;
    char* chunk;

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

    uint8_t* plo_miss = malloc(nsamples * sizeof(uint8_t));

    for (uint32_t i = 0; i < nsamples; ++i) {
        plo_miss[i] = (uint8_t)c[i];
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

    return 0;
}

void bgen_layout2_read_genotype(struct bgen_genotype* vg, double* p)
{
    if (vg->phased)
        read_phased_genotype(vg, p);
    else
        read_unphased_genotype(vg, p);
}

static void read_phased_genotype(struct bgen_genotype* genotype, double* probabilities)
{
    uint8_t  nbits = genotype->nbits;
    uint16_t nalleles = genotype->nalleles;
    uint8_t  max_ploidy = genotype->max_ploidy;
    double   denom = (double)((((uint64_t)1 << nbits)) - 1);

    uint64_t sample_start = 0;
    for (uint32_t j = 0; j < genotype->nsamples; ++j) {
        uint8_t ploidy = read_ploidy(genotype->plo_miss[j]);
        double* pend = probabilities + max_ploidy * nalleles;

        if (read_missingness(genotype->plo_miss[j]) != 0) {
            set_array_nan(probabilities, (size_t)(pend - probabilities));
            probabilities = pend;
            sample_start += nbits * ploidy * (nalleles - 1);
            continue;
        }

        uint64_t haplo_start = 0;
        for (uint8_t i = 0; i < ploidy; ++i) {

            uint64_t uip_sum = 0;
            uint64_t allele_start = 0;
            for (uint16_t ii = 0; ii < nalleles - 1; ++ii) {

                uint64_t ui_prob = 0;
                uint64_t offset = sample_start + haplo_start + allele_start;

                for (uint8_t bi = 0; bi < nbits; ++bi) {

                    if (get_bit(genotype->current_chunk, bi + offset)) {
                        ui_prob |= ((uint64_t)1 << bi);
                    }
                }

                *probabilities = ui_prob / denom;
                ++probabilities;
                uip_sum += ui_prob;
                allele_start += nbits;
            }
            *probabilities = (denom - uip_sum) / denom;
            ++probabilities;
            haplo_start += nbits * (nalleles - 1);
        }
        sample_start += nbits * ploidy * (nalleles - 1);
        set_array_nan(probabilities, (size_t)(pend - probabilities));
        probabilities = pend;
    }
}

static void read_unphased_genotype(struct bgen_genotype* genotype, double* probabilities)
{
    uint8_t  nbits = genotype->nbits;
    uint16_t nalleles = genotype->nalleles;
    uint8_t  max_ploidy = genotype->max_ploidy;

    double   denom = (double)((((uint64_t)1 << nbits)) - 1);
    unsigned max_ncombs = choose(nalleles + max_ploidy - 1, nalleles - 1);

    uint64_t sample_start = 0;
    for (uint32_t j = 0; j < genotype->nsamples; ++j) {
        double*  pend = probabilities + max_ncombs;
        uint8_t  ploidy = read_ploidy(genotype->plo_miss[j]);
        unsigned ncombs = choose(nalleles + ploidy - 1, nalleles - 1);

        if (read_missingness(genotype->plo_miss[j]) != 0) {
            set_array_nan(probabilities, (size_t)(pend - probabilities));
            probabilities = pend;
            sample_start += nbits * (ncombs - 1);
            continue;
        }

        uint64_t uip_sum = 0;

        uint64_t geno_start = 0;
        for (uint8_t i = 0; i < (size_t)(ncombs - 1); ++i) {

            uint64_t ui_prob = 0;
            uint64_t offset = sample_start + geno_start;

            for (uint8_t bi = 0; bi < (size_t)nbits; ++bi) {

                if (get_bit(genotype->current_chunk, bi + offset)) {
                    ui_prob |= ((uint64_t)1 << bi);
                }
            }

            *probabilities = ui_prob / denom;
            ++probabilities;
            uip_sum += ui_prob;
            geno_start += nbits;
        }
        *probabilities = (denom - uip_sum) / denom;
        ++probabilities;
        sample_start += nbits * (ncombs - 1);
        set_array_nan(probabilities, (size_t)(pend - probabilities));
        probabilities = pend;
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
