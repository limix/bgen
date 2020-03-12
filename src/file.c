#include "file.h"
#include "athr.h"
#include "bgen/file.h"
#include "bgen/genotype.h"
#include "free.h"
#include "genotype.h"
#include "io.h"
#include "layout1.h"
#include "layout2.h"
#include "report.h"
#include "samples.h"
#include "str.h"
#include <inttypes.h>
#include <stdbool.h>

struct bgen_file
{
    char*    filepath;
    FILE*    stream;
    uint32_t nvariants;
    uint32_t nsamples;
    unsigned compression;
    unsigned layout;
    bool     contain_sample;
    int64_t  samples_start;
    int64_t  variants_start;
};

static struct bgen_file* bgen_file_create(char const* filepath);
static int               read_bgen_header(struct bgen_file* bgen);

struct bgen_file* bgen_file_open(char const* filepath)
{
    struct bgen_file* bgen = bgen_file_create(filepath);
    if (bgen == NULL)
        return NULL;

    if (fread_i64(bgen->stream, &bgen->variants_start, 4)) {
        bgen_error("could not read the `variants_start` field");
        goto err;
    }

    bgen->variants_start += 4;

    if (read_bgen_header(bgen)) {
        bgen_error("could not read bgen header");
        goto err;
    }

    /* if they actually exist */
    if ((bgen->samples_start = bgen_ftell(bgen->stream)) < 0) {
        bgen_error("could not ftell");
        goto err;
    }

    return bgen;

err:
    bgen_file_close(bgen);
    return NULL;
}

void bgen_file_close(struct bgen_file const* bgen)
{
    if (bgen->stream != NULL && fclose(bgen->stream))
        bgen_perror("could not close %s file", bgen->filepath);
    free_c(bgen->filepath);
    free_c(bgen);
}

uint32_t bgen_file_nsamples(struct bgen_file const* bgen) { return bgen->nsamples; }

uint32_t bgen_file_nvariants(struct bgen_file const* bgen) { return bgen->nvariants; }

bool bgen_file_contain_samples(struct bgen_file const* bgen) { return bgen->contain_sample; }

struct bgen_samples* bgen_file_read_samples(struct bgen_file* bgen, int verbose)
{
    struct athr* at = NULL;

    if (bgen_fseek(bgen->stream, bgen->samples_start, SEEK_SET)) {
        bgen_error("could not fseek to `samples_start`");
        return NULL;
    }

    if (!bgen->contain_sample) {
        bgen_warning("file does not contain sample ids");
        return NULL;
    }

    struct bgen_samples* samples = bgen_samples_create(bgen->nsamples);

    if (bgen_fseek(bgen->stream, 8, SEEK_CUR)) {
        bgen_perror("could not fseek eight bytes forward");
        goto err;
    }

    if (verbose) {
        at = athr_create((long)bgen->nsamples, "Reading samples", ATHR_BAR);
        if (!at) {
            bgen_error("could not create a progress bar");
            goto err;
        }
    }

    for (uint32_t i = 0; i < bgen->nsamples; ++i) {
        if (verbose)
            athr_consume(at, 1);

        struct bgen_str const* sample_id = bgen_str_fread(bgen->stream, 2);
        if (sample_id == NULL) {
            bgen_error("could not read the %" PRIu32 "lu-th sample id", i);
            goto err;
        }
        bgen_samples_set(samples, i, sample_id);
    }

    if (verbose)
        athr_finish(at);

    if ((bgen->variants_start = bgen_ftell(bgen->stream)) < 0) {
        bgen_error("could not ftell `variants_start`");
        goto err;
    }

    return samples;

err:
    if (at)
        athr_finish(at);
    bgen_samples_free(samples);
    return NULL;
}

struct bgen_genotype* bgen_file_open_genotype(struct bgen_file* bgen, uint64_t variant_offset)
{
    struct bgen_genotype* geno = bgen_genotype_create();
    geno->layout = bgen->layout;
    geno->offset = variant_offset;

    if (variant_offset > INT64_MAX) {
        bgen_error("variant offset overflow");
        goto err;
    }

    if (bgen_fseek(bgen_file_stream(bgen), (int64_t)variant_offset, SEEK_SET)) {
        bgen_perror("could not seek a variant in %s", bgen_file_filepath(bgen));
        goto err;
    }

    if (bgen_file_layout(bgen) == 1) {
        bgen_layout1_read_header(bgen, geno);
    } else if (bgen_file_layout(bgen) == 2) {
        bgen_layout2_read_header(bgen, geno);
    } else {
        bgen_error("unrecognized layout type %d", bgen_file_layout(bgen));
        goto err;
    }

    return geno;
err:
    bgen_genotype_close(geno);
    return NULL;
}

FILE* bgen_file_stream(struct bgen_file const* bgen_file) { return bgen_file->stream; }

char const* bgen_file_filepath(struct bgen_file const* bgen_file)
{
    return bgen_file->filepath;
}

unsigned bgen_file_layout(struct bgen_file const* bgen_file) { return bgen_file->layout; }

unsigned bgen_file_compression(struct bgen_file const* bgen_file)
{
    return bgen_file->compression;
}

int bgen_file_seek_variants_start(struct bgen_file* bgen_file)
{
    if (bgen_fseek(bgen_file->stream, bgen_file->variants_start, SEEK_SET)) {
        bgen_perror("could not jump to variants start");
        return 1;
    }
    return 0;
}

static struct bgen_file* bgen_file_create(char const* filepath)
{
    struct bgen_file* bgen = malloc(sizeof(struct bgen_file));
    bgen->filepath = strdup(filepath);
    bgen->stream = NULL;
    bgen->nvariants = 0;
    bgen->nsamples = 0;
    bgen->compression = 0;
    bgen->layout = 0;
    bgen->contain_sample = 0;
    bgen->samples_start = 0;
    bgen->variants_start = 0;

    if (!(bgen->stream = fopen(bgen->filepath, "rb"))) {
        bgen_perror("could not open bgen file %s", bgen->filepath);
        bgen_file_close(bgen);
        return NULL;
    }

    return bgen;
}

/*
 * Read the header block defined as follows:
 *
 *   header length: 4 bytes
 *   number of variants: 4 bytes
 *   number of samples: 4 bytes
 *   magic number: 4 bytes
 *   unused space: header length minus 20 bytes
 *   bgen flags: 4 bytes
 */
static int read_bgen_header(struct bgen_file* bgen)
{
    uint32_t header_length;
    uint32_t magic_number;
    uint32_t flags;

    if (fread_ui32(bgen->stream, &header_length, 4)) {
        bgen_error("could not read header length");
        return 1;
    }

    if (fread_ui32(bgen->stream, &bgen->nvariants, 4)) {
        bgen_error("could not read number of variants");
        return 1;
    }

    if (fread_ui32(bgen->stream, &bgen->nsamples, 4)) {
        bgen_error("could not read number of samples");
        return 1;
    }

    if (fread_ui32(bgen->stream, &magic_number, 4)) {
        bgen_error("could not read magic number");
        return 1;
    }

    if (magic_number != 1852139362)
        bgen_warning("magic number mismatch");

    if (bgen_fseek(bgen->stream, header_length - 20, SEEK_CUR)) {
        bgen_error("fseek error while reading bgen file");
        return 1;
    }

    if (fread_ui32(bgen->stream, &flags, 4)) {
        bgen_error("could not read bgen flags");
        return 1;
    }

    bgen->compression = flags & 3;
    bgen->layout = (flags & (15 << 2)) >> 2;
    bgen->contain_sample = ((flags & ((uint32_t)1 << 31)) >> 31) == 1;

    return 0;
}
