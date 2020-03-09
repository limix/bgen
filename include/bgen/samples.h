#ifndef BGEN_SAMPLES_H
#define BGEN_SAMPLES_H

#include "bgen/str.h"
#include "bgen_export.h"
#include <stdint.h>

/** Bgen samples.
 * @struct bgen_samples
 */
struct bgen_samples;

BGEN_EXPORT void                   bgen_samples_free(struct bgen_samples const* samples);
BGEN_EXPORT struct bgen_str const* bgen_samples_get(struct bgen_samples const* samples,
                                                    uint32_t                   index);

#endif
