#include "bgen_reader.h"
#include "file.h"
#include "util.h"

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
    char *fp = bgenfile->filepath;
    FILE *f  = fopen(fp, "rb");

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    fseek(f, bgenfile->variants_start, SEEK_SET);

    if (bgen_reader_layout(bgenfile) == 0)
    {
        if (fread_check(&(vb->nsamples), 4, f, fp)) return EXIT_FAILURE;
    }

    if (fread_check(&(vb->id_length), 2, f, fp)) return EXIT_FAILURE;

    vb->id = malloc(vb->id_length);

    if (fread_check(vb->id, vb->id_length, f, fp)) return EXIT_FAILURE;

    if (fread_check(&(vb->rsid_length), 2, f, fp)) return EXIT_FAILURE;

    vb->rsid = malloc(vb->rsid_length);

    if (fread_check(vb->rsid, vb->rsid_length, f, fp)) return EXIT_FAILURE;

    if (fread_check(&(vb->chrom_length), 2, f, fp)) return EXIT_FAILURE;

    vb->chrom = malloc(vb->chrom_length);

    if (fread_check(vb->chrom, vb->chrom_length, f, fp)) return EXIT_FAILURE;

    if (fread_check(&(vb->position), 4, f, fp)) return EXIT_FAILURE;

    if (bgen_reader_layout(bgenfile) == 1) vb->nalleles = 2;
    else if (fread_check(&(vb->nalleles), 2, f, fp)) return EXIT_FAILURE;

    size_t i;
    vb->alleles = malloc(vb->nalleles * sizeof(Allele));

    for (i = 0; i < vb->nalleles; ++i)
    {
        if (fread_check(&(vb->alleles[i].length), 4, f, fp)) return EXIT_FAILURE;

        vb->alleles[i].id = malloc(vb->alleles[i].length);

        if (fread_check(vb->alleles[i].id, vb->alleles[i].length, f, fp)) return EXIT_FAILURE;
    }

    vb->genotype_start = ftell(f);

    fclose(f);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_id(BGenFile *bgenfile, uint64_t idx, BYTE **id,
                               uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.id_length;
    *id     = vb.id;

    fclose(f);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_rsid(BGenFile *bgenfile, uint64_t idx, BYTE **rsid,
                                 uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.rsid_length;
    *rsid   = vb.rsid;

    fclose(f);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_chrom(BGenFile *bgenfile, uint64_t idx, BYTE **chrom,
                                  uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    *length = vb.chrom_length;
    *chrom  = vb.chrom;

    fclose(f);

    return EXIT_SUCCESS;
}

int64_t bgen_reader_variant_position(BGenFile *bgenfile,
                                     uint64_t  idx)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return -1;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return -1;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    fclose(f);

    return vb.position;
}

int64_t bgen_reader_variant_nalleles(BGenFile *bgenfile,
                                     uint64_t  idx)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return -1;
    }

    if (idx >= bgen_reader_nvariants(bgenfile)) return -1;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx, &vb);

    fclose(f);

    return vb.nalleles;
}

int64_t bgen_reader_variant_allele_id(BGenFile *bgenfile, uint64_t idx0,
                                      uint64_t idx1, BYTE **id,
                                      uint64_t *length)
{
    FILE *f = fopen(bgenfile->filepath, "rb");

    if (!f) {
        fprintf(stderr, "File opening failed: %s\n", bgenfile->filepath);
        return EXIT_FAILURE;
    }

    if (idx0 >= bgen_reader_nvariants(bgenfile)) return -1;

    if (idx1 >= bgen_reader_variant_nalleles(bgenfile, idx0)) return EXIT_FAILURE;

    VariantBlock vb;

    bgen_reader_variant_block(bgenfile, idx0, &vb);

    *id = vb.alleles[idx1].id;

    fclose(f);

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
int64_t _genotype_block_layout1(FILE *f, char *fp, int64_t compression,
                                int64_t nsamples, double **probabilities)
{
    uint64_t clength;
    BYTE    *cchunk, *uchunk;

    size_t ulength = 6 * nsamples;

    uchunk = malloc(ulength);

    if (compression == 0)
    {
        if (fread_check(uchunk, ulength, f, fp)) return EXIT_FAILURE;
    } else {
        if (fread_check(&clength, 4, f, fp)) return EXIT_FAILURE;

        cchunk = malloc(clength);

        if (fread_check(cchunk, clength, f, fp)) return EXIT_FAILURE;

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
int64_t _genotype_block_layout2(FILE *f, char *fp, int64_t compression,
                                int64_t nsamples, double **probabilities)
{
    uint64_t clength, ulength;
    BYTE    *cchunk, *uchunk;

    if (compression == 0)
    {
        if (fread_check(&ulength, 4, f, fp)) return EXIT_FAILURE;

        uchunk = malloc(ulength);

        if (fread_check(uchunk, ulength, f, fp)) return EXIT_FAILURE;
    } else {
        if (fread_check(&clength, 4, f, fp)) return EXIT_FAILURE;
        if (fread_check(&ulength, 4, f, fp)) return EXIT_FAILURE;

        cchunk = malloc(clength);
        uchunk = malloc(ulength);

        if (fread_check(cchunk, clength - 4, f, fp)) return EXIT_FAILURE;

        zlib_uncompress(cchunk, clength, &uchunk, &ulength);
    }

    return EXIT_SUCCESS;
}

int64_t bgen_reader_genotype_block(BGenFile *bgenfile, uint64_t idx,
                                   VariantBlock *vb)
{
    bgen_reader_variant_block(bgenfile, idx, vb);

    char *fp = bgenfile->filepath;
    FILE *f  = fopen(fp, "rb");

    fseek(f, vb->genotype_start, SEEK_SET);

    int64_t layout = bgen_reader_layout(bgenfile);
    double *probabilities;

    if (layout == 1)
    {
        _genotype_block_layout1(f, fp, bgen_reader_compression(bgenfile),
                                bgen_reader_nsamples(bgenfile),
                                &probabilities);
    } else if (layout == 2) {
        _genotype_block_layout2(f, fp, bgen_reader_compression(bgenfile),
                                bgen_reader_nsamples(bgenfile),
                                &probabilities);
    }
    fclose(f);

    return EXIT_SUCCESS;
}

// // compressed_data contains the (compressed or uncompressed) probability
// data.
// uint32_t const compressionType = (context.flags &
// bgen::e_CompressedSNPBlocks) ;
// if( compressionType != e_NoCompression ) {
//     byte_t const* begin = &compressed_data[0] ;
//     byte_t const* const end = &compressed_data[0] + compressed_data.size() ;
//     uint32_t uncompressed_data_size = 0 ;
//     if( (context.flags & e_Layout) == e_Layout1 ) {
//         uncompressed_data_size = 6 * context.number_of_samples ;
//     } else {
//         begin = read_little_endian_integer( begin, end,
// &uncompressed_data_size ) ;
//     }
//     buffer->resize( uncompressed_data_size ) ;
//     if( compressionType == e_ZlibCompression ) {
//         zlib_uncompress( begin, end, buffer ) ;
//     } else if( compressionType == e_ZstdCompression ) {
//         zstd_uncompress( begin, end, buffer ) ;
//     }
//     assert( buffer->size() == uncompressed_data_size ) ;
// }
// else {
//     // copy the data between buffers.
//     buffer->assign( compressed_data.begin(), compressed_data.end() ) ;
// }
