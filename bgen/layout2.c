#include <stdlib.h>
#include <assert.h>

#include "layout2.h"

#include "bgen.h"
#include "util/file.h"
#include "util/bits.h"
#include "util/zlib_wrapper.h"
#include "util/zstd_wrapper.h"
#include "util/mem.h"
#include "util/choose.h"

#define FAIL EXIT_FAILURE

inline static inti bgen_read_ploidy(byte ploidy_miss)
{
    return ploidy_miss & 127;
}

inline static inti bgen_read_missingness(byte ploidy_miss)
{
    return (ploidy_miss & 256) >> 7;
}

inline static inti bit_sample_start(inti sample_idx, inti nbits,
                                    inti ncomb)
{
    return sample_idx * (nbits * (ncomb - 1));
}

inline static inti bit_geno_start(inti geno_idx, inti nbits)
{
    return geno_idx * nbits;
}

inline static int get_bit(const byte *mem, inti bit_idx)
{
    inti bytes = bit_idx / 8;

    return GetBit(*(mem + bytes), bit_idx % 8);
}

inti bgen_read_unphased_genotype(const byte      *chunk,
                                 VariantGenotype *vg,
                                 inti             nsamples,
                                 inti             nalleles,
                                 inti             nbits,
                                 uint8_t         *plo_miss)
{
    inti ncombs = bgen_choose(nalleles + vg->ploidy - 1, nalleles - 1);

    vg->probabilities = malloc(ncombs * nsamples * sizeof(real));
    vg->ncombs        = ncombs;

    real denom = (((inti)1 << nbits)) - 1;

    inti i, j, bi;

    for (j = 0; j < nsamples; ++j)
    {
        inti ploidy = bgen_read_ploidy(plo_miss[j]);
        assert(bgen_read_missingness(plo_miss[j]) == 0);

        ncombs = bgen_choose(nalleles + ploidy - 1, nalleles - 1);

        // printf("ncombs inside: %lld\n", ncombs);

        inti uip_sum = 0;

        for (i = 0; i < ncombs - 1; ++i)
        {
            inti ui_prob = 0;

            for (bi = 0; bi < nbits; ++bi)
            {
                inti sample_start = bit_sample_start(j, nbits, ncombs);
                inti geno_start   = bit_geno_start(i, nbits);
                inti bit_idx      = sample_start + geno_start + bi;

                // printf("bit_idx: %lld\n", bit_idx);

                if (get_bit(chunk, bit_idx))
                {
                    // printf("got bit\n");
                    ui_prob |= ((inti)1 << bi);
                }
            }

            // printf("%lld\n", ui_prob);
            vg->probabilities[j * ncombs + i] = ui_prob / denom;
            uip_sum                          += ui_prob;
        }
        vg->probabilities[j * ncombs + ncombs - 1] = (denom - uip_sum) / denom;
    }
    return EXIT_SUCCESS;
}

byte* bgen_uncompress(VariantIndexing *indexing)
{
    inti  clength = 0, ulength = 0;
    byte *cchunk;
    byte *uchunk;

    if (bgen_read(indexing->file, &clength, 4)) return NULL;

    clength -= 4;

    if (bgen_read(indexing->file, &ulength, 4)) return NULL;

    cchunk = malloc(clength);

    if (bgen_read(indexing->file, cchunk, clength))
    {
        free(cchunk);
        return NULL;
    }

    uchunk = malloc(ulength);

    if (indexing->compression == 1)
    {
        printf("COMPRESSION 1\n");
        zlib_uncompress(cchunk, clength, &uchunk, &ulength);
    }

    if (indexing->compression == 2)
    {
        printf("COMPRESSION 2\n");
        zstd_uncompress(cchunk, clength, &uchunk, &ulength);
    }


    free(cchunk);

    return uchunk;
}

inti bgen_read_layout2_genotype(VariantIndexing *indexing,
                                VariantGenotype *vg)
{
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t  min_ploidy, max_ploidy, phased, nbits;

    byte *c;
    byte *chunk;

    if (indexing->compression > 0)
    {
        chunk = bgen_uncompress(indexing);
        c     = chunk;
        MEMCPY(&nsamples, &c, 4);
    }
    else {
        if (bgen_read(indexing->file, &nsamples, 4)) return FAIL;

        chunk = malloc(6 * nsamples);

        if (bgen_read(indexing->file, chunk, 6 * nsamples)) return FAIL;

        c = chunk;
    }

    MEMCPY(&nalleles,   &c, 2);
    MEMCPY(&min_ploidy, &c, 1);
    MEMCPY(&max_ploidy, &c, 1);
    vg->ploidy = max_ploidy;

    uint8_t *plo_miss = malloc(nsamples * sizeof(uint8_t));

    inti i;

    for (i = 0; i < nsamples; ++i)
    {
        plo_miss[i] = c[i];
    }
    c += nsamples;

    MEMCPY(&phased, &c, 1);
    MEMCPY(&nbits,  &c, 1);

    assert(phased == 0);

    bgen_read_unphased_genotype(c, vg, nsamples, nalleles, nbits, plo_miss);

    free(chunk);
    free(plo_miss);

    return EXIT_SUCCESS;
}
