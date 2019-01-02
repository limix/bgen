#define BGEN_API_EXPORTS

#include "file.h"
#include "athr.h"
#include "bgen.h"
#include "mem.h"
#include "str.h"
#include <assert.h>

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
int read_bgen_header(struct bgen_file *bgen)
{
    assert(bgen);

    uint32_t header_length;
    uint32_t magic_number;
    uint32_t flags;

    if (fread_ui32(bgen->file, &header_length, 4)) {
        error("Could not read the header length.");
        return 1;
    }

    if (fread_int(bgen->file, &bgen->nvariants, 4)) {
        error("Could not read the number of variants.");
        return 1;
    }

    if (fread_int(bgen->file, &bgen->nsamples, 4)) {
        error("Could not read the number of samples.");
        return 1;
    }

    if (fread_ui32(bgen->file, &magic_number, 4)) {
        error("Could not read the magic number.");
        return 1;
    }

    if (magic_number != 1852139362) {
        error("This is not a BGEN file: magic number mismatch.");
        return 1;
    }

    if (fseek(bgen->file, header_length - 20, SEEK_CUR)) {
        error("Fseek error while reading a BGEN file.");
        return 1;
    }

    if (fread_ui32(bgen->file, &flags, 4)) {
        error("Could not read the bgen flags.");
        return 1;
    }

    bgen->compression = flags & 3;
    bgen->layout = (flags & (15 << 2)) >> 2;
    bgen->contain_sample = (flags & (1 << 31)) >> 31;

    return 0;
}

BGEN_API struct bgen_file *bgen_open(const char *filepath)
{
    assert(filepath);

    struct bgen_file *bgen = dalloc(sizeof(struct bgen_file));
    if (!bgen)
        goto err;

    bgen->samples_start = -1;
    bgen->variants_start = -1;
    bgen->file = NULL;

    bgen->filepath = strdup(filepath);

    bopen_or_leave(bgen);

    if (fread_int(bgen->file, &bgen->variants_start, 4))
        goto err;

    bgen->variants_start += 4;

    if (read_bgen_header(bgen)) {
        error("Could not read bgen header.");
        goto err;
    }

    /* if they actually exist */
    bgen->samples_start = ftell(bgen->file);

    if (fclose(bgen->file)) {
        perror_fmt("Could not close %s", filepath);
        goto err;
    }

    return bgen;

err:
    if (bgen) {
        fclose_nul(bgen->file);
        free_nul(bgen->filepath);
    }
    return free_nul(bgen);
}

BGEN_API void bgen_close(struct bgen_file *bgen)
{
    if (bgen)
        free_nul(bgen->filepath);
    free_nul(bgen);
}

BGEN_API int bgen_nsamples(const struct bgen_file *bgen)
{
    assert(bgen);
    return bgen->nsamples;
}

BGEN_API int bgen_nvariants(const struct bgen_file *bgen)
{
    assert(bgen);
    return bgen->nvariants;
}

BGEN_API int bgen_contain_sample(const struct bgen_file *bgen)
{
    assert(bgen);
    return bgen->contain_sample;
}

BGEN_API struct bgen_str *bgen_read_samples(struct bgen_file *bgen, int verbose)
{
    assert(bgen);

    struct bgen_str *sample_ids = NULL;
    struct athr *at = NULL;

    bopen_or_leave(bgen);
    fseek(bgen->file, bgen->samples_start, SEEK_SET);

    if (bgen->contain_sample == 0) {
        error("This bgen file does not contain sample ids.");
        goto err;
    }

    sample_ids = dalloc(bgen->nsamples * sizeof(struct bgen_str));

    if (fseek(bgen->file, 8, SEEK_CUR))
        goto err;

    if (verbose)
        at = athr_create(bgen->nsamples, "Reading samples", ATHR_BAR);

    for (size_t i = 0; i < (size_t)bgen->nsamples; ++i) {
        if (verbose)
            athr_consume(at, 1);

        if (fread_str(bgen->file, sample_ids + i, 2))
            goto err;
    }

    if (verbose)
        athr_finish(at);

    bgen->variants_start = ftell(bgen->file);
    fclose(bgen->file);

    return sample_ids;

err:
    fclose_nul(bgen->file);
    if (at)
        athr_finish(at);
    return free_nul(sample_ids);
}

BGEN_API void bgen_free_samples(const struct bgen_file *bgen, struct bgen_str *samples)
{
    assert(bgen);
    assert(samples);

    if (bgen->contain_sample == 0)
        return;

    if (samples == NULL)
        return;

    for (size_t i = 0; i < (size_t)bgen->nsamples; ++i)
        free_nul(samples[i].str);

    free(samples);
}
