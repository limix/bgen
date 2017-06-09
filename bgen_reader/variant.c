#include "bgen_reader.h"
#include "file.h"
#include "util.h"
#include "list.h"
#include "bits.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

// Variant identifying data
//
// ---------------------------------------------
// | 4     | # samples (layout > 1)            |
// | 2     | # variant id length, Lid          |
// | Lid   | variant id                        |
// | 2     | variant rsid length, Lrsid        |
// | Lrsid | rsid                              |
// | 2     | chrom length, Lchr                |
// | Lchr  | chromossome                       |
// | 4     | variant position                  |
// | 2     | number of alleles, K (layout > 1) |
// | 4     | first allele length, La1          |
// | La1   | first allele                      |
// | 4     | second allele length, La2         |
// | La2   | second allele                     |
// | ...   |                                   |
// | 4     | last allele length, LaK           |
// | LaK   | last allele                       |
// ---------------------------------------------
int64_t bgen_reader_variant_block(BGenFile *bgenfile, uint64_t idx,
                                  VariantBlock *vb)
{
    printf("Inside bgen_reader_variant_block\n");
    printf("Point 1\n");

    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    printf("Point 2\n");

    printf("nvariants %llu\n", bgen_reader_nvariants(bgenfile));


    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    printf("Point 3\n");
    printf("variants_start: %ld\n", bgenfile->variants_start);

    fseek(bgenfile->file, bgenfile->variants_start, SEEK_SET);

    printf("Point 4\n");

    printf("Layout: %llu\n", bgen_reader_layout(bgenfile));

    assert(bgen_reader_layout(bgenfile) != 0);

    if (bgen_reader_layout(bgenfile) == 1)
    {
        if (fread_check(bgenfile, &(vb->nsamples), 4)) return EXIT_FAILURE;
    }
    printf("Point 5\n");

    if (fread_check(bgenfile, &(vb->id_length), 2)) return EXIT_FAILURE;
    printf("Point 6\n");
    printf("id_length %hu\n", vb->id_length);

    vb->id = malloc(vb->id_length);

    if (fread_check(bgenfile, vb->id, vb->id_length)) return EXIT_FAILURE;
    printf("Point 7\n");
    printf("id %s\n", vb->id);

    if (fread_check(bgenfile, &(vb->rsid_length), 2)) return EXIT_FAILURE;
    printf("Point 8\n");

    vb->rsid = malloc(vb->rsid_length);

    if (fread_check(bgenfile, vb->rsid, vb->rsid_length)) return EXIT_FAILURE;

    printf("Point 9\n");

    if (fread_check(bgenfile, &(vb->chrom_length), 2)) return EXIT_FAILURE;

    printf("Point 10\n");

    vb->chrom = malloc(vb->chrom_length);

    if (fread_check(bgenfile, vb->chrom, vb->chrom_length)) return EXIT_FAILURE;

    printf("Point 11\n");

    if (fread_check(bgenfile, &(vb->position), 4)) return EXIT_FAILURE;

    printf("Point 12\n");

    if (bgen_reader_layout(bgenfile) == 1) vb->nalleles = 2;
    else if (fread_check(bgenfile, &(vb->nalleles), 2)) return EXIT_FAILURE;

    printf("Point 13\n");

    size_t i;
    vb->alleles = malloc(vb->nalleles * sizeof(Allele));

    printf("nalleles: %d\n", vb->nalleles);

    for (i = 0; i < vb->nalleles; ++i)
    {
        if (fread_check(bgenfile, &(vb->alleles[i].length), 4)) return EXIT_FAILURE;

        vb->alleles[i].id = malloc(vb->alleles[i].length);

        if (fread_check(bgenfile, vb->alleles[i].id, vb->alleles[i].length)) return EXIT_FAILURE;
    }

    printf("Point 14\n");

    vb->genotype_start = ftell(bgenfile->file);

    printf("Point 15\n");

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    printf("Point 16\n");

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variantid(BGenFile *bgenfile, uint64_t idx, BYTE **id,
                              uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    printf("Ponto 1\n");
    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;
    printf("Ponto 2\n");

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);
    printf("Ponto 3\n");

    *length = vb.id_length;
    *id     = vb.id;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    printf("Ponto 4\n");

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_rsid(BGenFile *bgenfile, uint64_t idx, BYTE **rsid,
                                 uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.rsid_length;
    *rsid   = vb.rsid;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_chrom(BGenFile *bgenfile, uint64_t idx, BYTE **chrom,
                                  uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.chrom_length;
    *chrom  = vb.chrom;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t  idx)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (idx >= bgen_reader_nvariants(bgenfile)) return -1;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return vb.position;
}

int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t  idx)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (idx >= bgen_reader_nvariants(bgenfile)) return -1;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return vb.nalleles;
}

int64_t bgen_reader_variant_alleleid(BGenFile *bgenfile, uint64_t idx0,
                                      uint64_t idx1, BYTE **id,
                                      uint64_t *length)
{
    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    if (idx0 >= bgen_reader_nvariants(bgenfile)) return -1;

    if (idx1 >= bgen_reader_variant_nalleles(bgenfile, idx0)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx0, &vb);

    *id = vb.alleles[idx1].id;

    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

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
int64_t _genotype_block_layout1(BGenFile *bgenfile, int64_t compression,
                                int64_t nsamples, double **probabilities)
{
    uint32_t clength;
    BYTE    *cchunk, *uchunk;

    uint32_t ulength = 6 * nsamples;

    uchunk = malloc(ulength);

    if (compression == 0)
    {
        if (fread_check(bgenfile, uchunk, ulength)) return EXIT_FAILURE;
    } else {
        if (fread_check(bgenfile, &clength, 4)) return EXIT_FAILURE;

        cchunk = malloc(clength);

        if (fread_check(bgenfile, cchunk, clength)) return EXIT_FAILURE;

        zlib_uncompress(cchunk, clength, &uchunk, &ulength);

        free(cchunk);
    }

    uint16_t *ui_uchunk = (uint16_t *)uchunk;

    size_t nprobs = 3 * nsamples;
    *probabilities = malloc(sizeof(double) * nprobs);

    size_t i;

    for (i = 0; i < nprobs; ++i)
    {
        (*probabilities)[i] = ((double)ui_uchunk[i]) / 32768;
    }

    free(uchunk);

    return EXIT_SUCCESS;
}

inline static uint8_t _read_ploidy(BYTE ploidy_miss)
{
    return ploidy_miss & 127;
}

inline static uint8_t _read_missingness(BYTE ploidy_miss)
{
    return (ploidy_miss & 256) >> 7;
}

int64_t _read_phased_probabilities(const BYTE *chunk, uint8_t nbits,
                                   uint8_t max_ploidy, BYTE *ploidy_miss,
                                   uint32_t nsamples, struct node **root)
{
    BYTE   *G;
    size_t  i, j;
    uint8_t ploidy;
    uint8_t miss;

    for (i = 0; i < max_ploidy; ++i)
    {
        for (j = 0; j < nsamples; ++j)
        {
            ploidy = _read_ploidy(ploidy_miss[j]);
            miss   = _read_missingness(ploidy_miss[j]);
        }
    }
    return EXIT_SUCCESS;
}

int64_t _read_unphased_probabilities(const BYTE *chunk, uint8_t nbits,
                                     uint8_t max_ploidy, BYTE *ploidy_miss,
                                     uint32_t nsamples, uint16_t nalleles,
                                     struct node **root)
{
    double  *g = malloc(100);
    size_t   i, j;
    size_t   bi;
    uint8_t  ploidy;
    uint8_t  miss;
    uint32_t ui_prob;

    uint32_t ncomb = choose(nalleles + max_ploidy - 1, nalleles - 1);

    printf("nalleles: %u\n",   nalleles);
    printf("max_ploidy: %u\n", max_ploidy);
    printf("ncomb: %u\n",      ncomb);

    for (j = 0; j < nsamples / 100; ++j)
    {
        ploidy = _read_ploidy(ploidy_miss[j]);
        printf("ploidy for ith sample: %u\n", ploidy);
        miss = _read_missingness(ploidy_miss[j]);
        printf("miss for ith sample: %u\n",   miss);

        if (miss)
        {
            // pass
        } else {
            ncomb   = choose(nalleles + ploidy - 1, nalleles - 1);
            ui_prob = 0;

            for (i = 0; i < ncomb - 1; ++i)
            {
                for (bi = 0; bi < nbits; ++bi)
                {
                    // pass
                    // if (GetBit(chunk + bit_to_byte_idx(bi), bit_in_byte_idx(bi))) SetBit(ui_prob, bit_in_byte_idx(bi));
                }

                // g[j] = transform(ui_prob);
            }

            // push(root, &g, 4);
        }

        //     //chunk += STEP ENTIRE INDIVIDUAL;
    }
    return EXIT_SUCCESS;
}

int64_t _probabilities_block_layout2(BYTE *chunk)
{
    uint32_t nsamples;
    uint16_t nalleles;
    uint8_t  min_ploidy, max_ploidy;
    size_t   i;

    MEMCPY(&nsamples,   &chunk, 4);

    MEMCPY(&nalleles,   &chunk, 2);

    MEMCPY(&min_ploidy, &chunk, 1);

    MEMCPY(&max_ploidy, &chunk, 1);

    BYTE *ploidy_miss = malloc(nsamples);

    MEMCPY(ploidy_miss, &chunk, nsamples);

    uint8_t phased;
    MEMCPY(&phased,     &chunk, 1);

    uint8_t nbits;
    MEMCPY(&nbits,      &chunk, 1);

    struct node *root;

    if (phased)
    {
        printf("PHASED\n");
        _read_phased_probabilities(chunk, nbits, max_ploidy, ploidy_miss,
                                   nsamples, &root);
    } else {
        printf("UNPHASED\n");
        _read_unphased_probabilities(chunk, nbits, max_ploidy, ploidy_miss,
                                     nsamples, nalleles, &root);

        // pass
        // _read_unphased_probabilities(chunk);
    }

    // for (i = 0; i < nsamples; ++i)
    // {
    //
    // }
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
int64_t _genotype_block_layout2(BGenFile *bgenfile, int64_t compression,
                                int64_t nsamples, double **probabilities)
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

        if (fread_check(bgenfile, &ulength, 4)) return EXIT_FAILURE;

        cchunk = malloc(clength);
        uchunk = malloc(ulength);

        if (fread_check(bgenfile, cchunk, clength - 4)) return EXIT_FAILURE;

        zlib_uncompress(cchunk, clength, &uchunk, &ulength);

        free(cchunk);
    }

    _probabilities_block_layout2(uchunk);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_genotype_block(BGenFile *bgenfile, uint64_t idx,
                                   VariantBlock *vb)
{
    bgen_reader_variant_block(bgenfile, idx, vb);

    if (bgen_fopen(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    fseek(bgenfile->file, vb->genotype_start, SEEK_SET);

    int64_t layout = bgen_reader_layout(bgenfile);
    double *probabilities;

    if (layout == 1)
    {
        _genotype_block_layout1(bgenfile, bgen_reader_compression(bgenfile),
                                bgen_reader_nsamples(bgenfile),
                                &probabilities);
    } else if (layout == 2) {
        _genotype_block_layout2(bgenfile, bgen_reader_compression(bgenfile),
                                bgen_reader_nsamples(bgenfile),
                                &probabilities);
    }
    if (bgen_fclose(bgenfile) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
