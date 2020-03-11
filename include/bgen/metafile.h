#ifndef BGEN_METAFILE_H
#define BGEN_METAFILE_H

#include "bgen_export.h"
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
 * addresses. Those variants are divided in partitions.
 *
 * @param bgen Bgen file handler.
 * @param filepath File path to the metafile.
 * @param npartitions Number of partitions as to be between `1` and the number of
 * samples.
 * @param verbose `1` for showing progress; `0` otherwise.
 * @return Metafile handler. `NULL` on failure.
 */
BGEN_EXPORT struct bgen_metafile* bgen_metafile_create(struct bgen_file* bgen,
                                                       char const*       filepath,
                                                       unsigned npartitions, int verbose);
/** Open a bgen metafile.
 *
 * @param filepath File path to the metafile.
 * @return Metafile handler. `NULL` on failure.
 */
BGEN_EXPORT struct bgen_metafile* bgen_metafile_open(char const* filepath);
/** Get the number of partitions.
 *
 * @param mf Metafile handler.
 * @return Number of partitions.
 */
BGEN_EXPORT uint32_t bgen_metafile_npartitions(struct bgen_metafile const* mf);
/** Get the number of variants.
 *
 * @param mf Metafile handler.
 * @return Number of variants.
 */
BGEN_EXPORT uint32_t bgen_metafile_nvariants(struct bgen_metafile const* mf);
/** Read a partition of variants.
 *
 * @param mf Metafile handler.
 * @param index Partition index.
 * @param nvariants Number of variants of the partition.
 * @return Array of variant metadata.
 */
/* BGEN_EXPORT struct bgen_variant_metadata* bgen_metafile_read_partition( */
/*     struct bgen_mf const* mf, uint32_t index, uint32_t* nvariants); */

BGEN_EXPORT struct bgen_partition* bgen_metafile_read_partition(
    struct bgen_metafile const* metafile, uint32_t partition);
/** Free a partition.
 *
 * @param vm Array of variant metatada.
 * @param nvariants Number of variants of the partition.
 */
BGEN_EXPORT void bgen_free_partition(struct bgen_variant* vm, uint32_t nvariants);
/** Close a metafile handler.
 *
 * @param mf Metafile handler.
 * @return `0` on success; `1` otherwise.
 */
BGEN_EXPORT int bgen_metafile_close(struct bgen_metafile* mf);

#endif
