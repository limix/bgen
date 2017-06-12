#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bgen_reader.h"
#include "bgen_file.h"
#include "variant.h"
#include "sample.h"
#include "file.h"
#include "util.h"


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
int bgen_read_sampleid_block(BGenFile *bgenfile)
{
    SampleIdBlock *block = bgenfile->sampleid_block;

    if (bgen_reader_fread(bgenfile, &(block->length), 4)) return EXIT_FAILURE;

    if (bgen_reader_fread(bgenfile, &(block->nsamples), 4)) return EXIT_FAILURE;

    block->sampleids = malloc(block->nsamples * sizeof(SampleId));

    for (size_t i = 0; i < block->nsamples; i++)
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

BGenFile* bgen_reader_open(char *filepath)
{
    BGenFile *bgenfile = malloc(sizeof(BGenFile));

    bgenfile->filepath = ft_strdup(filepath);

    if (bgen_reader_fopen(bgenfile) == EXIT_FAILURE) return NULL;


    if (bgenfile->file == NULL) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return NULL;
    }

    uint32_t offset;

    // First four bytes (offset)
    if (bgen_reader_fread(bgenfile, &offset, 4)) return NULL;

    if (bgen_read_read_header(bgenfile, &(bgenfile->header))) return NULL;

    if (bgenfile->header.header_length > offset)
    {
        fprintf(stderr, "Header length is larger then offset's.\n");
        bgen_reader_fclose(bgenfile);
        return NULL;
    }

    if (bgen_reader_sampleids(bgenfile))
    {
        bgenfile->sampleid_block = malloc(sizeof(SampleIdBlock));

        if (bgen_read_sampleid_block(bgenfile) == EXIT_FAILURE)
        {
            bgen_reader_fclose(bgenfile);
            return NULL;
        }
    }

    bgenfile->variants_start = ftell(bgenfile->file);

    if (bgenfile->variants_start == EOF)
    {
        perror("Could not find variant blocks");
        bgen_reader_fclose(bgenfile);
        return NULL;
    }

    if (bgen_reader_fclose(bgenfile) == EXIT_FAILURE) return NULL;

    return bgenfile;
}

int64_t bgen_reader_close(BGenFile *bgenfile)
{
    assert(bgenfile->file == NULL);

    free(bgenfile->filepath);
    bgenfile->filepath = NULL;

    assert(bgenfile->sampleid_block != NULL);
    free(bgenfile->sampleid_block);
    bgenfile->sampleid_block = NULL;

    free(bgenfile);
    return EXIT_SUCCESS;
}

// What layout is that?
// Possible values are 1 and 2.
int64_t bgen_reader_layout(BGenFile *bgenfile)
{
    int64_t l = (bgenfile->header.flags & (15 << 2)) >> 2;

    assert(l != 0);
    return l;
}

// Is sample identifier block present?
int64_t bgen_reader_sampleids(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & (1 << 31)) >> 31;
}

// Does it use compression? Which type?
// 0: no compression
// 1: zlib's compress()
// 2: zstandard's ZSTD_compress()
int64_t bgen_reader_compression(BGenFile *bgenfile)
{
    return (bgenfile->header.flags & 60) >> 2;
}

int64_t bgen_reader_nsamples(BGenFile *bgenfile)
{
    return bgenfile->header.nsamples;
}

int64_t bgen_reader_nvariants(BGenFile *bgenfile)
{
    return bgenfile->header.nvariants;
}

int64_t bgen_reader_read_probabilities(BGenFile *bgenfile, uint64_t variant_idx,
                                       uint32_t *ui_probs)
{
    VariantBlock vb;

    bgen_reader_read_variantid_block(bgenfile, variant_idx, &vb);

    if (bgen_reader_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    fseek(bgenfile->file, vb.genotype_start, SEEK_SET);

    int64_t e = bgen_reader_read_current_genotype_block(bgenfile, ui_probs);
    if (e == EXIT_FAILURE) return EXIT_FAILURE;

    if (bgen_reader_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
