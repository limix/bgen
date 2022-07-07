/** Open, close, and query metafile.
 * @file bgen/metafile.h
 */
#ifndef BGEN_METAFILE_H
#define BGEN_METAFILE_H

#include "bgen/export.h"
#include <inttypes.h>

struct bgen_file;
/** Metafile handler.
 * @struct bgen_metafile
 */
struct bgen_metafile;
struct bgen_variant;
struct bgen_partition;

/** Create a bgen metafile.
 *
 * A bgen metafile contains variant metadata (id, rsid, chrom, alleles) and variant
 * addresses. Those variants are grouped in partitions.
 *
 * @param bgen_file Bgen file handler.
 * @param filepath File path to the metafile.
 * @param npartitions Number of partitions. It has to be a number between `1` and
 * the number of samples.
 * @param verbose `1` for showing progress; `0` otherwise.
 * @return Metafile handler. `NULL` on failure.
 */
BGEN_EXPORT struct bgen_metafile* bgen_metafile_create(struct bgen_file* bgen_file,
                                                       char const*       filepath,
                                                       uint32_t npartitions, int verbose);
/** Open a bgen metafile.
 *
 * Remember to call @ref bgen_metafile_close to close the file and release
 * resources after the interaction has finished.
 *
 * @param filepath File path to the metafile.
 * @return Metafile handler. `NULL` on failure.
 */
BGEN_EXPORT struct bgen_metafile* bgen_metafile_open(char const* filepath);
/** Get the number of partitions.
 *
 * @param metafile Metafile handler.
 * @return Number of partitions.
 */
BGEN_EXPORT uint32_t bgen_metafile_npartitions(struct bgen_metafile const* metafile);
/** Get the number of variants.
 *
 * @param metafile Metafile handler.
 * @return Number of variants.
 */
BGEN_EXPORT uint32_t bgen_metafile_nvariants(struct bgen_metafile const* metafile);
/** Read a partition of variants.
 *
 * Remember to call @ref bgen_partition_destroy to release resources after the
 * interaction has finished.
 *
 * @param metafile Metafile handler.
 * @param partition Partition index.
 * @return Partition of variants. Return `NULL` on failure.
 */
BGEN_EXPORT struct bgen_partition const* bgen_metafile_read_partition(
    struct bgen_metafile const* metafile, uint32_t partition);
/** Close a metafile handler.
 *
 * @param metafile Metafile handler.
 * @return `0` on success; `1` otherwise.
 */
BGEN_EXPORT int bgen_metafile_close(struct bgen_metafile const* metafile);

#endif
