/** Open, close, and query bgen file.
 * @file bgen/file.h
 */
#ifndef BGEN_FILE_H
#define BGEN_FILE_H

#include "bgen/export.h"
#include <stdbool.h>
#include <stdint.h>

/** Bgen file handler.
 * @struct bgen_file
 */
struct bgen_file;

/** Open bgen file and return a handler.
 *
 * Remember to call @ref bgen_file_close to close the file and release
 * resources after the interaction has finished.
 *
 * @param filepath File path to the bgen file.
 * @return Bgen file handler. Return `NULL` on failure.
 */
BGEN_EXPORT struct bgen_file* bgen_file_open(char const* filepath);
/** Close bgen file handler.
 *
 * @param bgen_file Bgen file handler.
 */
BGEN_EXPORT void bgen_file_close(struct bgen_file const* bgen_file);
/** Get the number of samples.
 *
 * @param bgen_file Bgen file handler.
 * @return Number of samples.
 */
BGEN_EXPORT uint32_t bgen_file_nsamples(struct bgen_file const* bgen_file);
/** Get the number of variants.
 *
 * @param bgen_file Bgen file handler.
 * @return Number of variants.
 */
BGEN_EXPORT uint32_t bgen_file_nvariants(struct bgen_file const* bgen_file);
/** Check if the file contain sample identifications.
 *
 * @param bgen_file Bgen file handler.
 * @return `true` if bgen file contains the sample ids; `false` otherwise.
 */
BGEN_EXPORT bool bgen_file_contain_samples(struct bgen_file const* bgen_file);
/** Return all sample identifications.
 *
 * @param bgen_file Bgen file handler.
 * @return Sample identifications. Return `NULL` on failure.
 */
BGEN_EXPORT struct bgen_samples* bgen_file_read_samples(struct bgen_file* bgen_file);
/** Open a variant for genotype queries.
 *
 * @param bgen_file Bgen file handler.
 * @param genotype_offset Genotype offset obtained from @ref bgen_variant.genotype_offset.
 * @return Variant genotype handler. Return `NULL` on failure.
 */
BGEN_EXPORT struct bgen_genotype* bgen_file_open_genotype(struct bgen_file* bgen_file,
                                                          uint64_t          genotype_offset);

#endif
