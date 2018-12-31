#define BGEN_API_EXPORTS

#include "file.h"
#include "bgen.h"
#include "mem.h"
#include <assert.h>

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

    if (bgen_read_header(bgen)) {
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

    for (size_t i = 0; i < bgen->nsamples; ++i) {
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

    for (size_t i = 0; i < bgen->nsamples; ++i)
        free_nul(samples[i].str);

    free(samples);
}
