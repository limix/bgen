#ifndef BGEN_SAMPLES_H
#define BGEN_SAMPLES_H

#include "bgen/export.h"
#include <stdint.h>

/** Bgen samples.
 * @struct bgen_samples
 */
struct bgen_samples;

struct bgen_string;

/** Destroy samples data by releasing its resources.
 *
 * @param samples Samples data.
 */
BGEN_EXPORT void bgen_samples_destroy(struct bgen_samples const* samples);
/** Get a specific sample.
 *
 * @param samples Samples data.
 * @param index Sample index.
 * @return Sample information. Return `NULL` on failure.
 */
BGEN_EXPORT struct bgen_string const* bgen_samples_get(struct bgen_samples const* samples,
                                                       uint32_t                   index);

#endif
