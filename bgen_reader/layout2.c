#include "bgen_reader.h"
#include "util.h"
#include "file.h"
#include "bits.h"

#include <stdlib.h>
#include <assert.h>

inline static uint8_t bgen_read_ploidy(BYTE ploidy_miss)
{
    return ploidy_miss & 127;
}

inline static uint8_t bgen_read_missingness(BYTE ploidy_miss)
{
    return (ploidy_miss & 256) >> 7;
}

inline static size_t bit_sample_start(size_t sample_idx, uint8_t nbits,
                                      uint32_t ncomb)
{
    return sample_idx * (nbits * (ncomb - 1));
}

inline static size_t bit_geno_start(size_t geno_idx, uint8_t nbits)
{
    return geno_idx * nbits;
}

inline static int get_bit(const BYTE *mem, size_t bit_idx)
{
    size_t bytes = bit_idx / 8;

    return GetBit(*(mem + bytes), bit_idx % 8);
}

inline static void set_bit(uint32_t *val, size_t bit_idx)
{
    SetBit(*val, bit_idx % 8);
}

int64_t bgen_read_unphased_probabilities(const BYTE        *chunk,
                                         VariantProbsBlock *vpb,
                                         uint32_t          *ui_probs)
{
    if (ui_probs == NULL) return EXIT_SUCCESS;

    size_t   i, j;
    size_t   bi;
    size_t   sample_start, geno_start, bit_idx;
    uint8_t  ploidy;
    uint8_t  miss;
    uint32_t ui_prob;
    uint32_t ui_prob_sum;

    uint32_t ncombs =
        choose(vpb->nalleles + vpb->max_ploidy - 1, vpb->nalleles - 1);

    vpb->sample_probs = malloc(sizeof(SampleProbs) * vpb->nsamples);

    // nsamples
    for (j = 0; j < vpb->nsamples; ++j)
    {
        ploidy = bgen_read_ploidy(vpb->missingness[j]);
        miss   = bgen_read_missingness(vpb->missingness[j]);
        assert(miss == 0);

        ncombs = choose(vpb->nalleles + ploidy - 1, vpb->nalleles - 1);

        ui_prob_sum = 0;

        // vpb->sample_probs[j].ui_probs = malloc((ncombs - 1) *
        // sizeof(uint32_t));

        for (i = 0; i < ncombs - 1; ++i)
        {
            ui_prob = 0;

            for (bi = 0; bi < vpb->nbits; ++bi)
            {
                sample_start = bit_sample_start(j, vpb->nbits, ncombs);
                geno_start   = bit_geno_start(i, vpb->nbits);
                bit_idx      = sample_start + geno_start + bi;

                if (get_bit(chunk, bit_idx)) SetBit(ui_prob, bi);
            }

            // vpb->sample_probs[j].ui_probs[i] = ui_prob;
            ui_probs[j * (ncombs - 1) + i] = ui_prob;
            ui_prob_sum                   += ui_prob;
        }
    }
    return EXIT_SUCCESS;
}

int64_t bgen_probabilities_block_layout2(BYTE              *chunk,
                                         VariantProbsBlock *vpb,
                                         uint32_t          *ui_probs)
{
    MEMCPY(&(vpb->nsamples),   &chunk, 4);

    MEMCPY(&(vpb->nalleles),   &chunk, 2);

    MEMCPY(&(vpb->min_ploidy), &chunk, 1);

    MEMCPY(&(vpb->max_ploidy), &chunk, 1);

    vpb->missingness = malloc(vpb->nsamples);

    MEMCPY(vpb->missingness, &chunk, vpb->nsamples);

    MEMCPY(&(vpb->phased),   &chunk, 1);

    MEMCPY(&(vpb->nbits),    &chunk, 1);

    assert(vpb->phased == 0);
    bgen_read_unphased_probabilities(chunk, vpb, ui_probs);

    return EXIT_SUCCESS;
}

// Genotype data block (Layout 2)
//
//   1) No compression
//     -------------------------------
//     | 4 | block length minus 4, C |
//     | C | genotype probabilities  |
//     -------------------------------
//
//   2) Compressed
//     -------------------------------------------
//     | 4   | block length minus 4, C           |
//     | 4   | uncompressed data length, D       |
//     | C-4 | compressed genotype probabilities |
//     -------------------------------------------
//
// Probabilities block
// ------------------------------------------------
// | 4     | # samples                            |
// | 2     | # alleles                            |
// | 1     | minimum ploidy                       |
// | 1     | maximum ploidy                       |
// | N     | ploidy and missigness of each sample |
// | 1     | phased or not                        |
// | 1     | # bits per probability               |
// | C+N+6 | probabilities for each genotype      |
// ------------------------------------------------
int64_t bgen_genotype_block_layout2(BGenFile     *bgenfile,
                                    int64_t       compression,
                                    int64_t       nsamples,
                                    VariantBlock *vb,
                                    uint32_t     *ui_probs)
{
    uint32_t clength, ulength;
    BYTE    *cchunk, *uchunk;

    if (compression == 0)
    {
        if (fread_check(bgenfile, &ulength, 4)) return EXIT_FAILURE;

        uchunk = malloc(ulength);

        if (fread_check(bgenfile, uchunk, ulength)) return EXIT_FAILURE;
    } else {
        if (fread_check(bgenfile, &clength, 4)) return EXIT_FAILURE;

        clength -= 4;

        if (fread_check(bgenfile, &ulength, 4)) return EXIT_FAILURE;

        cchunk = malloc(clength);

        if (fread_check(bgenfile, cchunk, clength)) return EXIT_FAILURE;

        uchunk = malloc(ulength);

        assert(compression == 2);

        zlib_uncompress(cchunk, clength, &uchunk, &ulength);

        free(cchunk);
    }

    vb->vpb = malloc(sizeof(VariantProbsBlock));
    bgen_probabilities_block_layout2(uchunk, vb->vpb, ui_probs);

    return EXIT_SUCCESS;
}

int64_t bgen_genotype_block_layout2_skip(BGenFile     *bgenfile,
                                         int64_t       compression,
                                         int64_t       nsamples)
{
    uint32_t length;

    if (compression == 0)
    {
        length = 6 * nsamples;
    } else {
        if (fread_check(bgenfile, &length, 4)) return EXIT_FAILURE;
    }

    fseek(bgenfile->file, length, SEEK_CUR);

    return EXIT_SUCCESS;
}
