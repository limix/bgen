#ifndef BGEN_SAMPLES_H_PRIVATE
#define BGEN_SAMPLES_H_PRIVATE

#include "bgen/samples.h"
#include <stdint.h>

struct bgen_samples* bgen_samples_create(uint32_t nsamples);
void                 bgen_samples_set(struct bgen_samples const* samples, uint32_t index,
                                      struct bgen_string const* sample_id);

#endif
