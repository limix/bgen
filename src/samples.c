#include "samples.h"
#include "bgen/bstring.h"
#include "free.h"

struct bgen_samples
{
    struct bgen_string const** sample_ids;
    uint32_t                   nsamples;
};

void bgen_samples_destroy(struct bgen_samples const* samples)
{
    for (uint32_t i = 0; i < samples->nsamples; ++i) {
        if (samples->sample_ids[i])
            bgen_string_destroy(samples->sample_ids[i]);
    }

    bgen_free(samples->sample_ids);
    bgen_free(samples);
}

struct bgen_string const* bgen_samples_get(struct bgen_samples const* samples, uint32_t index)
{
    return samples->sample_ids[index];
}

struct bgen_samples* bgen_samples_create(uint32_t nsamples)
{
    struct bgen_samples* samples = malloc(sizeof(struct bgen_samples));
    samples->sample_ids = malloc(sizeof(struct bgen_string*) * nsamples);
    samples->nsamples = nsamples;
    for (uint32_t i = 0; i < nsamples; ++i)
        samples->sample_ids[i] = NULL;
    return samples;
}

void bgen_samples_set(struct bgen_samples const* samples, uint32_t index,
                      struct bgen_string const* sample_id)
{
    samples->sample_ids[index] = sample_id;
}
