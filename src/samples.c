#include "samples.h"
#include "free.h"
#include "str.h"

struct bgen_samples
{
    struct bgen_str const** sample_ids;
    uint32_t                nsamples;
};

void bgen_samples_free(struct bgen_samples const* samples)
{
    for (uint32_t i = 0; i < samples->nsamples; ++i) {
        if (samples->sample_ids[i] != NULL)
            bgen_str_free(samples->sample_ids[i]);
    }

    free_c(samples->sample_ids);
    free_c(samples);
}

struct bgen_str const* bgen_samples_get(struct bgen_samples const* samples,
                                        uint32_t const             index)
{
    return samples->sample_ids[index];
}

struct bgen_samples* bgen_samples_create(uint32_t const nsamples)
{
    struct bgen_samples* samples = malloc(sizeof(struct bgen_samples));
    samples->sample_ids = malloc(sizeof(struct bgen_str*) * nsamples);
    samples->nsamples = nsamples;
    for (uint32_t i = 0; i < nsamples; ++i)
        samples->sample_ids[i] = NULL;
    return samples;
}

void bgen_samples_set(struct bgen_samples const* samples, uint32_t const index,
                      struct bgen_str const* sample_id)
{
    samples->sample_ids[index] = sample_id;
}
