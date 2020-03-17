#include "layout2.h"
#include "bmath.h"
#include "file.h"
#include "genotype.h"
#include "mem.h"
#include "zip/zlib.h"
#include "zip/zstd.h"
#include <inttypes.h>
#include <math.h>

#define BIT(var, bit) ((var & (1 << bit)) != 0)

static void  read_phased_genotype(struct bgen_genotype* genotype, double* probabilities);
static void  read_unphased_genotype(struct bgen_genotype* genotype, double* probabilities);
static char* _uncompress(struct bgen_file* bgen_file);

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

int bgen_layout2_read_header(struct bgen_file* bgen_file, struct bgen_genotype* genotype)
{
    uint32_t nsamples = 0;
    uint16_t nalleles = 0;
    uint8_t  min_ploidy = 0, max_ploidy = 0, phased = 0, nbits = 0;

    char const* c;
    char*       chunk;

    if (bgen_file_compression(bgen_file) > 0) {
        if ((chunk = _uncompress(bgen_file)) == NULL)
            return 1;
        c = chunk;
        bgen_memfread(&nsamples, &c, 4);
    } else {
        if (fread(&nsamples, 1, 4, bgen_file_stream(bgen_file)) < 4)
            return 1;

        chunk = malloc(6 * nsamples);

        if (fread(chunk, 1, 6 * nsamples, bgen_file_stream(bgen_file)) < 6 * nsamples) {
            free(chunk);
            return 1;
        }

        c = chunk;
    }

    bgen_memfread(&nalleles, &c, 2);
    bgen_memfread(&min_ploidy, &c, 1);
    bgen_memfread(&max_ploidy, &c, 1);
    genotype->min_ploidy = min_ploidy;
    genotype->max_ploidy = max_ploidy;

    uint8_t* plo_miss = malloc(nsamples * sizeof(uint8_t));

    for (uint32_t i = 0; i < nsamples; ++i) {
        plo_miss[i] = (uint8_t)c[i];
    }
    c += nsamples;

    bgen_memfread(&phased, &c, 1);
    bgen_memfread(&nbits, &c, 1);

    genotype->nsamples = nsamples;
    genotype->nalleles = nalleles;
    genotype->phased = phased;
    genotype->nbits = nbits;
    genotype->ploidy_missingness = plo_miss;

    if (genotype->max_ploidy == 0) {
        bgen_error("`max_ploidy` cannot be zero");
        return 1;
    }
    if (genotype->nalleles == 0) {
        bgen_error("`nalleles` cannot be zero");
        return 1;
    }

    if (phased)
        genotype->ncombs = (unsigned)nalleles * (unsigned)genotype->max_ploidy;
    else
        genotype->ncombs = choose((unsigned)nalleles + (unsigned)(genotype->max_ploidy - 1),
                                  (unsigned)(nalleles - 1));

    genotype->chunk = chunk;
    genotype->chunk_ptr = c;

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
    unsigned nbits = genotype->nbits;
    unsigned nalleles = genotype->nalleles;
    uint8_t  max_ploidy = genotype->max_ploidy;
    double   denom = (double)((((uint64_t)1 << nbits)) - 1);

    uint64_t sample_start = 0;
    for (uint32_t j = 0; j < genotype->nsamples; ++j) {
        unsigned ploidy = read_ploidy(genotype->ploidy_missingness[j]);
        double*  pend = probabilities + max_ploidy * nalleles;

        if (read_missingness(genotype->ploidy_missingness[j]) != 0) {
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

                    if (get_bit(genotype->chunk_ptr, bi + offset)) {
                        ui_prob |= ((uint64_t)1 << bi);
                    }
                }

                *probabilities = (double)ui_prob / denom;
                ++probabilities;
                uip_sum += ui_prob;
                allele_start += nbits;
            }
            *probabilities = (denom - (double)uip_sum) / denom;
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
    unsigned max_ncombs =
        choose(nalleles + (unsigned)(max_ploidy - 1), (unsigned)(nalleles - 1));

    uint64_t sample_start = 0;
    for (uint32_t j = 0; j < genotype->nsamples; ++j) {
        double*  pend = probabilities + max_ncombs;
        uint8_t  ploidy = read_ploidy(genotype->ploidy_missingness[j]);
        unsigned ncombs = choose(nalleles + (unsigned)(ploidy - 1), (unsigned)(nalleles - 1));

        if (read_missingness(genotype->ploidy_missingness[j]) != 0) {
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

                if (get_bit(genotype->chunk_ptr, bi + offset)) {
                    ui_prob |= ((uint64_t)1 << bi);
                }
            }

            *probabilities = (double)ui_prob / denom;
            ++probabilities;
            uip_sum += ui_prob;
            geno_start += nbits;
        }
        *probabilities = (denom - (double)uip_sum) / denom;
        ++probabilities;
        sample_start += nbits * (ncombs - 1);
        set_array_nan(probabilities, (size_t)(pend - probabilities));
        probabilities = pend;
    }
}

static char* _uncompress(struct bgen_file* bgen_file)
{
    size_t clength, ulength;
    char*  cchunk;
    char*  uchunk;
    clength = 0;
    ulength = 0;

    if (fread(&clength, 1, 4, bgen_file_stream(bgen_file)) < 4)
        return NULL;

    clength -= 4;

    if (fread(&ulength, 1, 4, bgen_file_stream(bgen_file)) < 4)
        return NULL;

    cchunk = malloc(clength);

    if (fread(cchunk, 1, clength, bgen_file_stream(bgen_file)) < clength) {
        free(cchunk);
        return NULL;
    }

    uchunk = malloc(ulength);

    if (bgen_file_compression(bgen_file) == 1) {
        if (bgen_unzlib(cchunk, clength, &uchunk, &ulength)) {
            fprintf(stderr, "Failed while uncompressing chunk for layout 2.");
            goto err;
        }
    } else if (bgen_file_compression(bgen_file) == 2) {
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
