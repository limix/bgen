/** Open, close, and query bgen file.
 * @file bgen/file.h
 */
#ifndef BGEN_FILE_H
#define BGEN_FILE_H

#include "api.h"
#include "str.h"

/** Bgen file handler.
 * @struct bgen_file
 */
struct bgen_file;

/** Open a file and return a bgen file handler.
 *
 * Remember to call @ref bgen_close to close the file and release
 * resources after the interaction has finished.
 *
 * @param filepath File path to the bgen file.
 * @return Bgen file handler.
 */
BGEN_API struct bgen_file *bgen_open(const char *filepath);
/** Close a bgen file handler.
 *
 * @param bgen Bgen file handler.
 */
BGEN_API void bgen_close(struct bgen_file *bgen);
/** Get the number of samples.
 *
 * @param bgen Bgen file handler.
 * @return Number of samples.
 */
BGEN_API int bgen_nsamples(const struct bgen_file *bgen);
/** Get the number of variants.
 *
 * @param bgen Bgen file handler.
 * @return Number of variants.
 */
BGEN_API int bgen_nvariants(const struct bgen_file *bgen);
/** Check if the file contain sample identifications.
 *
 * \rst
 * .. seealso::
 *     Please, refer to the |bgen format specification| for more details.
 * \endrst
 *
 * @param bgen Bgen file handler.
 * @return `1` if bgen file contains the sample ids; `0` otherwise.
 */
BGEN_API int bgen_contain_samples(const struct bgen_file *bgen);
/** Return array of sample identifications.
 *
 * The size of this array is given be the @ref bgen_nsamples function.
 *
 * @param bgen Bgen file handler.
 * @param verbose `1` to show progress; `0` otherwise.
 * @return Array of sample ids.
 */
BGEN_API struct bgen_str *bgen_read_samples(struct bgen_file *bgen, int verbose);
/** Free array of sample identifications.
 *
 * @param bgen Bgen file handler.
 * @param sample_ids Array of sample ids.
 */
BGEN_API void bgen_free_samples(const struct bgen_file *bgen,
                                struct bgen_str *sample_ids);

#endif /* BGEN_FILE_H */
