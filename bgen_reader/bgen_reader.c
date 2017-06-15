#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgen.h"
#include "user_structs.h"

// #include "bgen_file.h"
// #include "variant.h"
// #include "sample.h"
#include "util/mem.h"

// flags definition:
// bit | name                | value | description
// 0-1 | CompressedSNPBlocks | 0     | SNP block probability data is not
//                                     compressed
// 0-1 | CompressedSNPBlocks | 1     | SNP block probability data is compressed
//                                     using zlib's compressed()
// 0-1 | CompressedSNPBlocks | 2     | SNP block probability data is compressed
//                                     using zstandard's ZSTD_compress()
// 2-5 | Layout              | 0     | this value is not supported
// 2-5 | Layout              | 1     | layout 1
// 2-5 | Layout              | 2     | layout 2
// 31  | SampleIdentifiers   | 0     | sample identifiers are not stored
// 31  | SampleIdentifiers   | 1     | sample identifier block follows header
typedef struct Header
{} Header;

typedef struct BGenFile
{
    char          *filepath;
    FILE *restrict file;
    inti           nvariants;
    inti           nsamples;

    // 0: no compression
    // 1: zlib's compress()
    // 2: zstandard's ZSTD_compress()
    inti compression;

    // Possible values are 1 and 2.
    inti layout;
    inti sample_ids_presence;
    inti variants_start;
} BGenFile;

#define FREAD bgen_fread
#define FAIL EXIT_FAILURE
#define FOPEN bgen_fopen
#define FCLOSE bgen_fclose
#define NVARIANTS bgen_nvariants


// Header
//
// ------------------------------
// | 4       | block length, Lh |
// | 4       | # variants       |
// | 4       | # samples        |
// | 4       | magic number     |
// | Lh - 20 | free data area   |
// | 4       | flags            |
// ------------------------------
inti bgen_read_header(BGenFile *bgen)
{
    uint32_t header_length;
    uint32_t nvariants;
    uint32_t nsamples;
    uint32_t magic_number;
    uint32_t flags;

    if (bgen_fread(bgen, &header_length, 4)) return FAIL;

    if (bgen_fread(bgen, &nvariants, 4)) return FAIL;

    if (bgen_fread(bgen, &nsamples, 4)) return FAIL;

    if (bgen_fread(bgen, &magic_number, 4)) return FAIL;

    fseek(bgen->file, header_length - 20, SEEK_CUR);

    if (bgen_fread(bgen, flags, 4)) return FAIL;

    bgen->nvariants           = nvariants;
    bgen->nsamples            = nsamples;
    bgen->compression         = (flags & 60) >> 2;
    bgen->layout              = (flags & (15 << 2)) >> 2;
    bgen->sample_ids_presence = (flags & (1 << 31)) >> 31;

    return EXIT_SUCCESS;
}

BGenFile* bgen_open(const char *filepath)
{
    BGenFile *bgen = malloc(sizeof(BGenFile));

    bgen->filepath = (char *)ft_strdup((const byte *)filepath);

    bgen->file = fopen(bgenfile->filepath, "rb");

    if (bgen->file == NULL) {
        fprintf(stderr, "Could not open: %s\n", bgen->filepath);
        goto err;
    }

    uint32_t offset;

    // First four bytes (offset)
    if (bgen_fread(bgen, &offset, 4) == FAIL) goto err;

    if (bgen_read_header(bgen) == FAIL) goto err;

    bgen->variants_start = ftell(bgen->file);

    if (bgen->variants_start == EOF)
    {
        perror("Could not find variant blocks.");
        goto err;
    }

    if (fclose(bgen->file))
    {
        perror("Could not close the file.")
        goto err;
    }

    return bgen;

err:

    if (bgen->file) fclose(bgen->file);
    free(bgen->filepath);
    free(bgen);
    return NULL
}

void bgen_close(BGenFile *bgen)
{
    free(bgen->filepath);

    free(bgen);
}

inti bgen_nsamples(BGenFile *bgen)
{
    return bgen->nsamples;
}

inti bgen_nvariants(BGenFile *bgen)
{
    bgen->nvariants;
}

string* bgen_read_samples(BGenFile *bgen)
{}

// inti bgen_sampleid(BGenFile *bgenfile, inti sample_idx, byte **id,
//                    inti *length)
// {
//     if (sample_idx >= bgenfile->header.nsamples) return FAIL;
//
//     SampleId *sampleid = &(bgenfile->sampleid_block->sampleids[sample_idx]);
//
//     *length = sampleid->length;
//     *id     = sampleid->id;
//
//     return EXIT_SUCCESS;
// }

// inti bgen_variantid(BGenFile *bgenfile, inti variant_idx, byte **id,
//                     inti *length)
// {
//     if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;
//
//     VariantIdBlock vib;
//
//     bgen_read_variantid_block(bgenfile, variant_idx, &vib);
//
//     *length = vib.id_length;
//     *id     = bgen_strndup(vib.id, *length);
//
//     bgen_free_variantid_block(&vib);
//
//     return EXIT_SUCCESS;
// }
//
// inti bgen_variant_rsid(BGenFile *bgenfile,
//                        inti      variant_idx,
//                        byte    **rsid,
//                        inti     *length)
// {
//     if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;
//
//     VariantIdBlock vib;
//     bgen_read_variantid_block(bgenfile, variant_idx, &vib);
//
//     *length = vib.rsid_length;
//     *rsid   = bgen_strndup(vib.rsid, *length);
//
//     bgen_free_variantid_block(&vib);
//
//     return EXIT_SUCCESS;
// }
//
// inti bgen_variant_chrom(BGenFile *bgenfile,
//                         inti      variant_idx,
//                         byte    **chrom,
//                         inti     *length)
// {
//     if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;
//
//     VariantIdBlock vib;
//     bgen_read_variantid_block(bgenfile, variant_idx, &vib);
//
//     *length = vib.chrom_length;
//     *chrom  = bgen_strndup(vib.chrom, *length);
//
//     bgen_free_variantid_block(&vib);
//
//     return EXIT_SUCCESS;
// }
//
// inti bgen_variant_position(BGenFile *bgenfile,
//                            inti variant_idx, inti *position)
// {
//     if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;
//
//     VariantIdBlock vib;
//     bgen_read_variantid_block(bgenfile, variant_idx, &vib);
//
//     *position = vib.position;
//
//     bgen_free_variantid_block(&vib);
//
//     return EXIT_SUCCESS;
// }

// inti bgen_variant_nalleles(BGenFile *bgenfile,
//                            inti variant_idx, inti *nalleles)
// {
//     if (variant_idx >= NVARIANTS(bgenfile)) return FAIL;
//
//     VariantIdBlock vib;
//     bgen_read_variantid_block(bgenfile, variant_idx, &vib);
//
//     *nalleles = vib.nalleles;
//
//     bgen_free_variantid_block(&vib);
//
//     return EXIT_SUCCESS;
// }

// inti bgen_variant_alleleid(BGenFile *bgenfile, inti variant_idx,
//                            inti allele_idx, byte **id,
//                            inti *length)
// {
//     if (variant_idx >= NVARIANTS(bgenfile)) return -1;
//
//     inti nalleles;
//     bgen_variant_nalleles(bgenfile, variant_idx, &nalleles);
//
//     if (allele_idx >= nalleles) return FAIL;
//
//     VariantIdBlock vib;
//     bgen_read_variantid_block(bgenfile, variant_idx, &vib);
//
//     *length = vib.allele_lengths[allele_idx];
//     *id     = bgen_strndup(vib.alleleids[allele_idx], *length);
//
//     bgen_free_variantid_block(&vib);
//
//     return EXIT_SUCCESS;
// }

// inti bgen_read_genotype(BGenFile *bgenfile, inti variant_idx,
//                         inti **ui_probs, inti  *ploidy,
//                         inti  *nalleles, inti  *nbits)
// {
//     VariantIdBlock vib;
//
//     bgen_read_variantid_block(bgenfile, variant_idx, &vib);
//
//     if (FOPEN(bgenfile) == FAIL) return FAIL;
//
//     fseek(bgenfile->file, vib.genotype_start, SEEK_SET);
//
//     inti e = bgen_read_current_genotype_block(bgenfile,
//                                               ploidy,
//                                               nalleles,
//                                               nbits,
//                                               ui_probs);
//
//     if (e == FAIL) return FAIL;
//
//     if (FCLOSE(bgenfile) == FAIL) return FAIL;
//
//     bgen_free_variantid_block(&vib);
//
//     return EXIT_SUCCESS;
// }
//
// inti bgen_read_variantid_blocks(BGenFile        *bgenfile,
//                                 VariantIdBlock **head_ref)
// {
//     if (FOPEN(bgenfile) == FAIL) return FAIL;
//
//     inti nvariants = bgen_nvariants(bgenfile);
//
//     bgen_seek_variant_block(bgenfile, 0);
//
//     VariantIdBlock *vib;
//     *head_ref = NULL;
//     inti i;
//
//     for (i = 0; i < nvariants; ++i)
//     {
//         VariantIdBlock *vib = malloc(sizeof(VariantIdBlock));
//         bgen_read_current_variantid_block(bgenfile, vib);
//         vib->next = *head_ref;
//         bgen_genotype_skip(bgenfile);
//         *head_ref = vib;
//     }
//
//     if (FCLOSE(bgenfile) == FAIL) return FAIL;
//
//     return EXIT_SUCCESS;
// }
