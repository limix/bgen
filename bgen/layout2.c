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

void bgen_read_unphased_genotype(VariantGenotype *vg,
                                 real            *probabilities)
{
    inti ncombs, ploidy, uip_sum, ui_prob;
    inti sample_start, geno_start, bit_idx;
    real denom = (((inti)1 << vg->nbits)) - 1;

    inti i, j, bi;

    for (j = 0; j < vg->nsamples; ++j)
    {
        ploidy = bgen_read_ploidy(vg->plo_miss[j]);
        assert(bgen_read_missingness(vg->plo_miss[j]) == 0);

        ncombs = bgen_choose(vg->nalleles + ploidy - 1, vg->nalleles - 1);

        uip_sum = 0;

        for (i = 0; i < ncombs - 1; ++i)
        {
            ui_prob = 0;

            for (bi = 0; bi < vg->nbits; ++bi)
            {
                sample_start = bit_sample_start(j, vg->nbits, ncombs);
                geno_start   = bit_geno_start(i, vg->nbits);
                bit_idx      = sample_start + geno_start + bi;


                if (get_bit(vg->current_chunk, bit_idx))
                {
                    ui_prob |= ((inti)1 << bi);
                }
            }

            probabilities[j * ncombs + i] = ui_prob / denom;
            uip_sum                      += ui_prob;
        }
        probabilities[j * ncombs + ncombs - 1] = (denom - uip_sum) / denom;
    }
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
        zlib_uncompress(cchunk, clength, &uchunk, &ulength);
    }

    if (indexing->compression == 2)
    {
        zstd_uncompress(cchunk, clength, &uchunk, &ulength);
    }


    free(cchunk);

    return uchunk;
}

inti bgen_read_variant_genotype_header(
    VariantIndexing *indexing,
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

    vg->nsamples      = nsamples;
    vg->nalleles      = nalleles;
    vg->nbits         = nbits;
    vg->plo_miss      = plo_miss;
    vg->ncombs        = bgen_choose(nalleles + vg->ploidy - 1, nalleles - 1);
    vg->chunk         = chunk;
    vg->current_chunk = c;

    return EXIT_SUCCESS;
}

void bgen_read_variant_genotype_probabilities(VariantIndexing *indexing,
                                              VariantGenotype *vg,
                                              real            *probabilities)
{
    bgen_read_unphased_genotype(vg, probabilities);
}
