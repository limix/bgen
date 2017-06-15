#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgen.h"

// #include "bgen_file.h"
// #include "variant.h"
// #include "sample.h"
#include "util/mem.h"

typedef struct Variant
{
    string  id;
    string  rsid;
    string  chrom;
    inti    position;
    inti    nalleles;
    string *allele_ids;
} Variant;

typedef struct VariantGenotype
{
    inti  ploidy;
    real *probabilities;
} VariantGenotype;

typedef struct VariantIndexing
{
    char *filename;
    inti  compression;
    inti  layout;
    inti *start;
} VariantIndexing;

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
    inti samples_start;
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

    bgen->samples_start  = -1;
    bgen->variants_start = -1;

    uint32_t offset;

    // First four bytes (offset)
    if (bgen_fread(bgen, &offset, 4) == FAIL) goto err;

    if (bgen_read_header(bgen) == FAIL) goto err;

    bgen->samples_start = ftell(bgen->file);

    if (bgen->samples_start == EOF)
    {
        perror("Could not find sample blocks.");
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
{
    if (bgen->sample_ids_presence == 0) return NULL;

    bgen->file = fopen(bgenfile->filepath, "rb");

    string *sample_ids = malloc(bgen->nsamples * sizeof(string));

    uint32_t length, nsamples;
    uint16_t len;

    if (bgen_fread(bgen, &length, 4)) goto err;

    if (bgen_fread(bgen, &nsamples, 4)) goto err;

    for (inti i = 0; i < bgen->nsamples; ++i)
    {
        if (bgen_fread(bgen, &len, 2)) goto err;

        sample_ids[i].str = malloc(len);
        sample_ids[i].len = len;

        if (bgen_fread(bgen, sample_ids[i].str, len)) return NULL;
    }

    if (fclose(bgen->file))
    {
        perror("Could not close the file.")
        goto err;
    }

    return sample_ids;

err:

    if (bgen->file) fclose(bgen->file);
    free(sample_ids);
    return NULL;
}

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
inti bgen_read_variant(BGenFile *bgen, Variant *v)
{
    uint32_t nsamples, position, allele_len;
    uint16_t id_len, rsid_len, chrom_len, nalleles;

    if (bgen->layout == 1)
    {
        if (bgen_fread(bgen, &nsamples, 4) == FAIL) return FAIL;
    }

    if (bgen_fread(bgen, &id_len, 2) == FAIL) return FAIL;

    string_alloc(v->id, id_len);

    if (bgen_fread(bgen, v->id.str, v->id.len) == FAIL) return FAIL;

    if (bgen_fread(bgen, rsid_len, 2) == FAIL) return FAIL;

    string_alloc(v->rsid, &rsid_len);

    if (bgen_fread(bgen, v->rsid.str, v->rsid.len) == FAIL) return FAIL;

    if (bgen_fread(bgen, &chrom_len, 2) == FAIL) return FAIL;

    string_alloc(v->chrom, chrom_len);

    if (bgen_fread(bgen, v->chrom.str, v->chrom.len) == FAIL) return FAIL;

    if (bgen_fread(bgen, &position, 4) == FAIL) return FAIL;


    if (bgen->layout == 1) nalleles = 2;
    else if (bgen_fread(bgen, &nalleles, 2) == FAIL) return FAIL;

    v->nalleles = nalleles;

    v->allele_ids = malloc(nalleles * sizeof(string));

    for (inti i = 0; i < v->nalleles; ++i)
    {
        if (bgen_read(bgen, &allele_len, 4) == FAIL) return FAIL;

        string_alloc(v->allele_ids + i, allele_len);

        if (bgen_read(bgen, v->allele_ids[i].str, allele_len) == FAIL) return FAIL;
    }

    // vib->genotype_start = ftell(bgen->file);

    return EXIT_SUCCESS;
}

Variant* bgen_read_variants(BGenFile *bgen, VariantIndexing *index)
{
    Variant *variants = NULL;

    bgen->file = fopen(bgen->filepath, "rb");

    fseek(bgen->);

    index->filepath    = bgen_strdup(bgen->filepath);
    index->compression = bgen->compression;
    index->layout      = bgen->layout;

    inti nvariants = bgen->nvariants;

    variants = malloc(nvariants * sizeof(Variant));


    for (inti i = 0; i < nvariants; ++i)
    {
        if (bgen_read_variant(bgen, variants + i) == FAIL) goto err;
    }

    if (fclose(bgen->file))
    {
        perror("Could not close the file.")
        goto err;
    }

err:

    if (bgen->file) fclose(bgen->file);

    if (variants != NULL) free(variants);
    return NULL;
}
