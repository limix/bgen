#include "file.h"
#include "athr.h"
#include "bgen/bgen.h"
#include "mem.h"
#include "str.h"
#include <assert.h>

struct bgen_file
{
    char* filepath;
    FILE* file;
    int nvariants;
    int nsamples;
    int compression;
    int layout;
    int contain_sample;
    OFF_T samples_start;
    OFF_T variants_start;
};

FILE* bgen_file_stream(struct bgen_file const* bgen_file) { return bgen_file->file; }

char const* bgen_file_filepath(struct bgen_file const* bgen_file)
{
    return bgen_file->filepath;
}

int bgen_file_layout(struct bgen_file const* bgen_file) { return bgen_file->layout; }

int bgen_file_compression(struct bgen_file const* bgen_file) { return bgen_file->compression; }

int bgen_file_seek_variants_start(struct bgen_file* bgen_file)
{
    if (LONG_SEEK(bgen_file->file, bgen_file->variants_start, SEEK_SET)) {
        perror_fmt(bgen_file->file, "Could not jump to the variants start");
        return 1;
    }
    return 0;
}

int close_bgen_file(struct bgen_file* bgen)
{
    char unknown[] = "`[unknown]`";
    const char* filepath = bgen->filepath ? bgen->filepath : unknown;

    if (fclose_nul(bgen->file)) {
        if (ferror(bgen->file))
            perror_fmt(bgen->file, "Could not close bgen file %s", filepath);
        else
            error("Could not close bgen file %s. Maybe it has already been closed", filepath);
        bgen->file = NULL;
        return 1;
    }
    bgen->file = NULL;
    return 0;
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
int read_bgen_header(struct bgen_file* bgen)
{
    uint32_t header_length;
    uint32_t magic_number;
    uint32_t flags;

    if (fread_ui32(bgen->file, &header_length, 4)) {
        error("Could not read the header length");
        return 1;
    }

    if (fread_int(bgen->file, &bgen->nvariants, 4)) {
        error("Could not read the number of variants");
        return 1;
    }

    if (fread_int(bgen->file, &bgen->nsamples, 4)) {
        error("Could not read the number of samples");
        return 1;
    }

    if (fread_ui32(bgen->file, &magic_number, 4)) {
        error("Could not read the magic number");
        return 1;
    }

    if (magic_number != 1852139362)
        warn("This might not be a BGEN file: magic number mismatch");

    if (LONG_SEEK(bgen->file, header_length - 20, SEEK_CUR)) {
        error("Fseek error while reading a BGEN file");
        return 1;
    }

    if (fread_ui32(bgen->file, &flags, 4)) {
        error("Could not read the bgen flags");
        return 1;
    }

    bgen->compression = flags & 3;
    bgen->layout = (flags & (15 << 2)) >> 2;
    bgen->contain_sample = (flags & ((uint32_t)1 << 31)) >> 31;

    return 0;
}

struct bgen_file* bgen_file_open(const char* filepath)
{
    struct bgen_file* bgen = dalloc(sizeof(struct bgen_file));
    if (!bgen)
        goto err;

    bgen->samples_start = -1;
    bgen->variants_start = -1;
    bgen->file = NULL;

    bgen->filepath = strdup(filepath);

    if (!(bgen->file = fopen(bgen->filepath, "rb"))) {
        perror_fmt(bgen->file, "Could not open bgen file %s", bgen->filepath);
        goto err;
    }

    if (fread_off(bgen->file, &bgen->variants_start, 4)) {
        error("Could not read the `variants_start` field");
        goto err;
    }

    bgen->variants_start += 4;

    if (read_bgen_header(bgen)) {
        error("Could not read bgen header");
        goto err;
    }

    /* if they actually exist */
    bgen->samples_start = LONG_TELL(bgen->file);

    return bgen;

err:
    if (bgen) {
        close_bgen_file(bgen);
        bgen->filepath = free_nul(bgen->filepath);
    }
    return free_nul(bgen);
}

void bgen_close(struct bgen_file* bgen)
{
    if (bgen) {
        close_bgen_file(bgen);
        bgen->filepath = free_nul(bgen->filepath);
    }
    free_nul(bgen);
}

int bgen_nsamples(struct bgen_file const* bgen) { return bgen->nsamples; }

int bgen_nvariants(struct bgen_file const* bgen) { return bgen->nvariants; }

int bgen_contain_samples(struct bgen_file const* bgen) { return bgen->contain_sample; }

struct bgen_str* bgen_read_samples(struct bgen_file* bgen, int verbose)
{
    struct bgen_str* sample_ids = NULL;
    struct athr* at = NULL;

    LONG_SEEK(bgen->file, bgen->samples_start, SEEK_SET);

    if (bgen->contain_sample == 0) {
        warn("This bgen file does not contain sample ids");
        return NULL;
    }

    sample_ids = dalloc((size_t)bgen->nsamples * sizeof(struct bgen_str));

    if (LONG_SEEK(bgen->file, 8, SEEK_CUR)) {
        perror("Could not fseek eight bytes forward");
        goto err;
    }

    if (verbose) {
        at = athr_create(bgen->nsamples, "Reading samples", ATHR_BAR);
        if (!at) {
            error("Could not create a progress bar");
            goto err;
        }
    }
    for (size_t i = 0; i < (size_t)bgen->nsamples; ++i) {
        if (verbose)
            athr_consume(at, 1);

        if (fread_str(bgen->file, sample_ids + i, 2)) {
            error("Could not read the %zu-th sample id", i);
            goto err;
        }
    }

    if (verbose)
        athr_finish(at);

    bgen->variants_start = LONG_TELL(bgen->file);
    return sample_ids;

err:
    if (at)
        athr_finish(at);
    return free_nul(sample_ids);
}

void bgen_free_samples(struct bgen_file const* bgen, struct bgen_str* samples)
{
    if (bgen->contain_sample == 0)
        return;

    if (!samples)
        return;

    for (size_t i = 0; i < (size_t)bgen->nsamples; ++i)
        free_str(samples + i);

    free(samples);
}
