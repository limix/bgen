#include <stdlib.h>
#include <assert.h>

#include "layout2.h"

#include "bgen.h"
#include "util/bits.h"
#include "util/zlib_wrapper.h"
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
                                 VariantGenotype *vg)
{
    inti i, j;
    inti bi;
    inti sample_start, geno_start, bit_idx;
    inti ploidy;
    inti miss;
    inti ui_prob;
    inti ui_prob_sum;
    inti tmp;

    inti ncombs =
        bgen_reader_choose(vpb->nalleles + vpb->max_ploidy - 1,
                           vpb->nalleles - 1);

    vpb->genotypes = calloc((ncombs - 1) * vpb->nsamples, sizeof(inti));

    // nsamples
    for (j = 0; j < vpb->nsamples; ++j)
    {
        ploidy = bgen_read_ploidy(vpb->missingness[j]);
        miss   = bgen_read_missingness(vpb->missingness[j]);
        assert(miss == 0);

        ncombs =
            bgen_reader_choose(vpb->nalleles + ploidy - 1, vpb->nalleles -
                               1);

        ui_prob_sum = 0;

        for (i = 0; i < ncombs - 1; ++i)
        {
            ui_prob = 0;

            for (bi = 0; bi < vpb->nbits; ++bi)
            {
                sample_start = bit_sample_start(j, vpb->nbits, ncombs);
                geno_start   = bit_geno_start(i, vpb->nbits);
                bit_idx      = sample_start + geno_start + bi;


                if (get_bit(chunk, bit_idx))
                {
                    tmp      = 1;
                    tmp    <<= bi;
                    ui_prob |= tmp;
                }
            }
            vpb->genotypes[j * (ncombs - 1) + i] = ui_prob;
            ui_prob_sum                         += ui_prob;
        }
    }
    return EXIT_SUCCESS;
}

inti bgen_reader_uncompress(VariantIndexing *indexing, byte **uchunk)
{
    inti  clength = 0, ulength = 0;
    byte *cchunk;

    if (indexing->compression == 0)
    {
        ulength = 6 * indexing->nsamples;

        *uchunk = malloc(ulength);

        if (bgen_read(indexing->file, *uchunk, ulength)) return FAIL;
    } else {
        if (bgen_read(indexing->file, &clength, 4)) return FAIL;

        clength -= 4;

        if (bgen_read(indexing->file, &ulength, 4)) return FAIL;

        cchunk = malloc(clength);

        if (bgen_read(indexing->file, cchunk, clength)) return FAIL;

        *uchunk = malloc(ulength);
        assert(compression == 2);
        zlib_uncompress(cchunk, clength, uchunk, &ulength);

        free(cchunk);
    }

    return EXIT_SUCCESS;
}

//
// // Genotype data block (Layout 2)
// //
// //   1) No compression
// //     -------------------------------
// //     | 4 | block length minus 4, C |
// //     | C | genotype probabilities  |
// //     -------------------------------
// //
// //   2) Compressed
// //     -------------------------------------------
// //     | 4   | block length minus 4, C           |
// //     | 4   | uncompressed data length, D       |
// //     | C-4 | compressed genotype probabilities |
// //     -------------------------------------------
// //
// // Probabilities block
// // ------------------------------------------------
// // | 4     | # samples                            |
// // | 2     | # alleles                            |
// // | 1     | minimum ploidy                       |
// // | 1     | maximum ploidy                       |
// // | N     | ploidy and missigness of each sample |
// // | 1     | phased or not                        |
// // | 1     | # bits per probability               |
// // | C+N+6 | probabilities for each genotype      |
// // ------------------------------------------------
inti bgen_read_layout2_genotype(VariantIndexing *indexing,
                                VariantGenotype *vg)
{
    byte *chunk;
    inti  e, i;

    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t  min_ploidy, max_ploidy, phased, nbits;

    e = bgen_reader_uncompress(indexing, &chunk);
    byte *c = chunk;

    if (e == FAIL) return FAIL;

    memset(vpb, 0, sizeof(VariantGenotypeBlock));

    MEMCPY(&nsamples,   &c, 4);
    MEMCPY(&nalleles,   &c, 2);
    MEMCPY(&min_ploidy, &c, 1);
    MEMCPY(&max_ploidy, &c, 1);

    vpb->missingness = malloc(nsamples * sizeof(inti));

    for (i = 0; i < vpb->nsamples; ++i)
    {
        vpb->missingness[i] = c[i];
    }
    c += vpb->nsamples;

    MEMCPY(&phased, &c, 1);
    MEMCPY(&nbits,  &c, 1);

    assert(phased == 0);
    bgen_read_unphased_genotype(c, vg);

    free(chunk);
    free(vpb->missingness);

    return EXIT_SUCCESS;
}

//
// inti bgen_reader_layout2_genotype_skip(BGenFile *bgenfile)
// {
//     inti length = 0;
//
//     if (bgen_reader_compression(bgenfile) == 0)
//     {
//         length = 6 * bgenfile->header.nsamples;
//     } else {
//         if (bgen_read(bgenfile, &length, 4)) return FAIL;
//     }
//
//     fseek(bgenfile->file, length, SEEK_CUR);
//
//     return EXIT_SUCCESS;
// }
