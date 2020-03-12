#ifndef BGEN_SAMPLES_H
#define BGEN_SAMPLES_H

#include "bgen/str.h"
#include "bgen_export.h"
#include <stdint.h>

/** Bgen samples.
 * @struct bgen_samples
 */
struct bgen_samples;

/** Destroy samples data.
 *
 * @param samples Samples data.
 */
BGEN_EXPORT void bgen_samples_destroy(struct bgen_samples const* samples);
/** Get a specific sample.
 *
 * @param samples Samples data.
 * @param index Sample index.
 * @return Sample information.
 */
BGEN_EXPORT struct bgen_string const* bgen_samples_get(struct bgen_samples const* samples,
                                                    uint32_t                   index);

#endif
