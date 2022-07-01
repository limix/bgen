#include "layout2.h"
#include "bmath.h"
#include "file.h"
#include "free.h"
#include "genotype.h"
#include "mem.h"
#include "unzlib.h"
#include "unzstd.h"
#include <inttypes.h>
#include <math.h>

#define BIT(var, bit) ((var & (1 << bit)) != 0)

static void  read_phased_genotype64(struct bgen_genotype* genotype, double* probs);
static void  read_phased_genotype32(struct bgen_genotype* genotype, float* probs);
static void  read_unphased_genotype64(struct bgen_genotype* genotype, double* probs);
static void  read_unphased_genotype32(struct bgen_genotype* genotype, float* probs);
static char* decompress(struct bgen_file* bgen_file);

static inline uint8_t read_ploidy(uint8_t ploidy_miss) { return ploidy_miss & 127; }

static inline int read_missingness(uint8_t ploidy_miss) { return ploidy_miss >> 7; }

static inline int get_bit(char const* mem, uint64_t bit_idx)
{
    uint64_t bytes = bit_idx / 8;
    return BIT(*(mem + bytes), bit_idx % 8);
}

static inline void set_array_nan64(double* p, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        p[i] = NAN;
}

static inline void set_array_nan32(float* p, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        p[i] = NAN;
}

int bgen_layout2_read_header(struct bgen_file* bgen_file, struct bgen_genotype* genotype)
{
    uint32_t nsamples = 0;
    uint8_t* plo_miss = NULL;

    char const* chunk_ptr = NULL;
    char*       chunk = NULL;

    if (bgen_file_compression(bgen_file) > 0) {

        if ((chunk = decompress(bgen_file)) == NULL) {
            goto err;
        }

        chunk_ptr = chunk;
        bgen_memfread(&nsamples, &chunk_ptr, sizeof(nsamples));

    } else {

        if (fread(&nsamples, sizeof(nsamples), 1, bgen_file_stream(bgen_file)) < 1) {
            bgen_perror_eof(bgen_file_stream(bgen_file), "could not read number of samples");
            goto err;
        }

        chunk = malloc(6 * nsamples);

        if (fread(chunk, 6 * nsamples, 1, bgen_file_stream(bgen_file)) < 1) {
            bgen_perror_eof(bgen_file_stream(bgen_file), "could not read chunk");
            goto err;
        }

        chunk_ptr = chunk;
    }

    uint16_t nalleles = 0;
    bgen_memfread(&nalleles, &chunk_ptr, 2);
    uint8_t min_ploidy = 0;
    bgen_memfread(&min_ploidy, &chunk_ptr, 1);
    uint8_t max_ploidy = 0;
    bgen_memfread(&max_ploidy, &chunk_ptr, 1);
    genotype->min_ploidy = min_ploidy;
    genotype->max_ploidy = max_ploidy;

    plo_miss = malloc(nsamples * sizeof(uint8_t));

    for (uint32_t i = 0; i < nsamples; ++i) {
        plo_miss[i] = (uint8_t)chunk_ptr[i];
    }
    chunk_ptr += nsamples;

    uint8_t phased = 0;
    bgen_memfread(&phased, &chunk_ptr, 1);
    uint8_t nbits = 0;
    bgen_memfread(&nbits, &chunk_ptr, 1);

    genotype->nsamples = nsamples;
    genotype->nalleles = nalleles;
    genotype->phased = phased;
    genotype->nbits = nbits;
    genotype->ploidy_missingness = plo_miss;

    if (phased && genotype->max_ploidy == 0) {
        bgen_error("phased genotype cannot have zero `max_ploidy`");
        goto err;
    }
    if (genotype->nalleles == 0) {
        bgen_error("`nalleles` cannot be zero");
        goto err;
    }

    if (phased)
        genotype->ncombs = (unsigned)nalleles * (unsigned)genotype->max_ploidy;
    else
        genotype->ncombs = choose((unsigned)(genotype->max_ploidy) + (unsigned)(nalleles - 1),
                                  (unsigned)(nalleles - 1));

    genotype->chunk = chunk;
    genotype->chunk_ptr = chunk_ptr;

    return 0;

err:
    bgen_free(chunk);
    bgen_free(plo_miss);
    genotype->chunk = NULL;
    genotype->ploidy_missingness = NULL;
    return 1;
}

void bgen_layout2_read_genotype64(struct bgen_genotype* genotype, double* probs)
{
    if (genotype->phased)
        read_phased_genotype64(genotype, probs);
    else
        read_unphased_genotype64(genotype, probs);
}

void bgen_layout2_read_genotype32(struct bgen_genotype* genotype, float* probs)
{
    if (genotype->phased)
        read_phased_genotype32(genotype, probs);
    else
        read_unphased_genotype32(genotype, probs);
}

#define MAKE_READ_PHASED_GENOTYPE(BITS, FPTYPE)                                               \
    static void read_phased_genotype##BITS(struct bgen_genotype* genotype, FPTYPE* probs)     \
    {                                                                                         \
        unsigned nbits = genotype->nbits;                                                     \
        unsigned nalleles = genotype->nalleles;                                               \
        uint8_t  max_ploidy = genotype->max_ploidy;                                           \
        FPTYPE   denom = (FPTYPE)((((uint64_t)1 << nbits)) - 1);                              \
                                                                                              \
        uint64_t sample_start = 0;                                                            \
        for (uint32_t j = 0; j < genotype->nsamples; ++j) {                                   \
            unsigned ploidy = read_ploidy(genotype->ploidy_missingness[j]);                   \
            FPTYPE*  pend = probs + max_ploidy * nalleles;                                    \
                                                                                              \
            if (read_missingness(genotype->ploidy_missingness[j]) != 0) {                     \
                set_array_nan##BITS(probs, (size_t)(pend - probs));                           \
                probs = pend;                                                                 \
                sample_start += nbits * ploidy * (nalleles - 1);                              \
                continue;                                                                     \
            }                                                                                 \
                                                                                              \
            uint64_t haplo_start = 0;                                                         \
            for (uint8_t i = 0; i < ploidy; ++i) {                                            \
                                                                                              \
                uint64_t uip_sum = 0;                                                         \
                uint64_t allele_start = 0;                                                    \
                for (uint16_t ii = 0; ii < nalleles - 1; ++ii) {                              \
                                                                                              \
                    uint64_t ui_prob = 0;                                                     \
                    uint64_t offset = sample_start + haplo_start + allele_start;              \
                                                                                              \
                    for (uint8_t bi = 0; bi < nbits; ++bi) {                                  \
                                                                                              \
                        if (get_bit(genotype->chunk_ptr, bi + offset)) {                      \
                            ui_prob |= ((uint64_t)1 << bi);                                   \
                        }                                                                     \
                    }                                                                         \
                                                                                              \
                    *probs = (FPTYPE)ui_prob / denom;                                         \
                    ++probs;                                                                  \
                    uip_sum += ui_prob;                                                       \
                    allele_start += nbits;                                                    \
                }                                                                             \
                *probs = (denom - (FPTYPE)uip_sum) / denom;                                   \
                ++probs;                                                                      \
                haplo_start += nbits * (nalleles - 1);                                        \
            }                                                                                 \
            sample_start += nbits * ploidy * (nalleles - 1);                                  \
            set_array_nan##BITS(probs, (size_t)(pend - probs));                               \
            probs = pend;                                                                     \
        }                                                                                     \
    }

MAKE_READ_PHASED_GENOTYPE(64, double)
MAKE_READ_PHASED_GENOTYPE(32, float)

#define MAKE_UNPHASED_GENOTYPE(BITS, FPTYPE)                                                  \
    static void read_unphased_genotype##BITS(struct bgen_genotype* genotype, FPTYPE* probs)   \
    {                                                                                         \
        uint8_t  nbits = genotype->nbits;                                                     \
        uint16_t nalleles = genotype->nalleles;                                               \
        uint8_t  max_ploidy = genotype->max_ploidy;                                           \
                                                                                              \
        FPTYPE   denom = (FPTYPE)((((uint64_t)1 << nbits)) - 1);                              \
        unsigned max_ncombs =                                                                 \
            choose(nalleles + (unsigned)(max_ploidy - 1), (unsigned)(nalleles - 1));          \
                                                                                              \
        uint64_t sample_start = 0;                                                            \
        for (uint32_t j = 0; j < genotype->nsamples; ++j) {                                   \
            FPTYPE*  pend = probs + max_ncombs;                                               \
            uint8_t  ploidy = read_ploidy(genotype->ploidy_missingness[j]);                   \
            unsigned ncombs =                                                                 \
                choose(nalleles + (unsigned)(ploidy - 1), (unsigned)(nalleles - 1));          \
                                                                                              \
            if (read_missingness(genotype->ploidy_missingness[j]) != 0) {                     \
                set_array_nan##BITS(probs, (size_t)(pend - probs));                           \
                probs = pend;                                                                 \
                sample_start += nbits * (ncombs - 1);                                         \
                continue;                                                                     \
            }                                                                                 \
                                                                                              \
            uint##BITS##_t uip_sum = 0;                                                       \
                                                                                              \
            uint64_t geno_start = 0;                                                          \
            for (uint8_t i = 0; i < (uint8_t)(ncombs - 1); ++i) {                             \
                                                                                              \
                uint##BITS##_t ui_prob = 0;                                                   \
                uint64_t       offset = sample_start + geno_start;                            \
                                                                                              \
                for (uint8_t bi = 0; bi < (uint8_t)nbits; ++bi) {                             \
                                                                                              \
                    if (get_bit(genotype->chunk_ptr, bi + offset)) {                          \
                        ui_prob |= ((uint##BITS##_t)1 << bi);                                 \
                    }                                                                         \
                }                                                                             \
                                                                                              \
                *probs = (FPTYPE)ui_prob / denom;                                             \
                ++probs;                                                                      \
                uip_sum += ui_prob;                                                           \
                geno_start += nbits;                                                          \
            }                                                                                 \
            *probs = (denom - (FPTYPE)uip_sum) / denom;                                       \
            ++probs;                                                                          \
            sample_start += nbits * (ncombs - 1);                                             \
            set_array_nan##BITS(probs, (size_t)(pend - probs));                               \
            probs = pend;                                                                     \
        }                                                                                     \
    }

MAKE_UNPHASED_GENOTYPE(64, double)
MAKE_UNPHASED_GENOTYPE(32, float)

static char* decompress(struct bgen_file* bgen_file)
{
    char* compressed_chunk = NULL;
    char* chunk = NULL;

    size_t compressed_length = 0;
    if (fread(&compressed_length, 4, 1, bgen_file_stream(bgen_file)) < 1) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read compressed length");
        goto err;
    }
    if (compressed_length < 4) {
        bgen_error("wrong compressed (corrupted file?)");
        goto err;
    }

    compressed_length -= 4;

    size_t length = 0;
    if (fread(&length, 4, 1, bgen_file_stream(bgen_file)) < 1) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read length");
        goto err;
    }

    compressed_chunk = malloc(compressed_length);
    if (compressed_chunk == NULL) {
        bgen_error("could not malloc compressed chunk");
        goto err;
    }

    if (fread(compressed_chunk, compressed_length, 1, bgen_file_stream(bgen_file)) < 1) {
        bgen_perror_eof(bgen_file_stream(bgen_file), "could not read chunk");
        goto err;
    }

    chunk = malloc(length);
    if (chunk == NULL) {
        bgen_error("could not malloc chunk");
        goto err;
    }

    if (bgen_file_compression(bgen_file) == 1) {
        if (bgen_unzlib(compressed_chunk, compressed_length, &chunk, &length))
            goto err;

    } else if (bgen_file_compression(bgen_file) == 2) {
        if (bgen_unzstd(compressed_chunk, compressed_length, (void**)&chunk, &length))
            goto err;

    } else {
        bgen_error("unrecognized compression method");
        goto err;
    }

    bgen_free(compressed_chunk);

    return chunk;

err:
    bgen_free(compressed_chunk);
    bgen_free(chunk);
    return NULL;
}
