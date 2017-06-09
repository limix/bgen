#include "bgen_reader.h"

#include <stdlib.h>

// Genotype data block (Layout 1)
//
//   1) No compression
//     -----------------------------------
//     | C = 6N | genotype probabilities |
//     -----------------------------------
//
//   2) Compressed
//     --------------------------------
//     | 4 | genotype chunk length, C |
//     | C | genotype probabilities   |
//     --------------------------------
int64_t bgen_genotype_block_layout1(BGenFile *bgenfile, int64_t compression,
                                int64_t nsamples, VariantBlock *vb)
{
    // uint32_t clength;
    // BYTE    *cchunk, *uchunk;
    //
    // uint32_t ulength = 6 * nsamples;
    //
    // uchunk = malloc(ulength);
    //
    // if (compression == 0)
    // {
    //     if (fread_check(bgenfile, uchunk, ulength)) return EXIT_FAILURE;
    // } else {
    //     if (fread_check(bgenfile, &clength, 4)) return EXIT_FAILURE;
    //
    //     cchunk = malloc(clength);
    //
    //     if (fread_check(bgenfile, cchunk, clength)) return EXIT_FAILURE;
    //
    //     zlib_uncompress(cchunk, clength, &uchunk, &ulength);
    //
    //     free(cchunk);
    // }
    //
    // uint16_t *ui_uchunk = (uint16_t *)uchunk;
    //
    // size_t nprobs = 3 * nsamples;
    // *probabilities = malloc(sizeof(double) * nprobs);
    //
    // size_t i;
    //
    // for (i = 0; i < nprobs; ++i)
    // {
    //     (*probabilities)[i] = ((double)ui_uchunk[i]) / 32768;
    // }
    //
    // free(uchunk);

    return EXIT_SUCCESS;
}
