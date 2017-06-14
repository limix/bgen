#include <stdlib.h>

#include "sample.h"
#include "bgen_file.h"

// Sample identifier block
//
// -------------------------------
// | 4   | block length          |
// | 4   | # samples             |
// | 2   | length of sample 1 id |
// | Ls1 | sample 1 id           |
// | 2   | length of sample 2 id |
// | Ls2 | sample 2 id           |
// | ... |                       |
// | 2   | length of sample N id |
// | LsN | sample N id           |
// -------------------------------
inti bgen_read_sampleid_block(BGenFile *bgenfile)
{
    SampleIdBlock *block = bgenfile->sampleid_block;

    if (bgen_reader_fread(bgenfile, &(block->length), 4)) return EXIT_FAILURE;

    if (bgen_reader_fread(bgenfile, &(block->nsamples), 4)) return EXIT_FAILURE;

    block->sampleids = malloc(block->nsamples * sizeof(SampleId));

    for (inti i = 0; i < block->nsamples; i++)
    {
        uint16_t *length = &(block->sampleids[i].length);

        if (bgen_reader_fread(bgenfile, length, 2)) return EXIT_FAILURE;

        block->sampleids[i].id =
            malloc(block->sampleids[i].length);

        if (bgen_reader_fread(bgenfile, block->sampleids[i].id,
                              block->sampleids[i].length)) return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
