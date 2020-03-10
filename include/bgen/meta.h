/** Create and query a metafile.
 * @file bgen/meta.h
 *
 * A bgen metafile is defined as follows:
 *
 * [ header block of 13 characters ], : "bgen index 03"
 * [ uint32_t : number of variants ],
 * [ uint64_t : metadata block size ],
 * [                                       \
 *   uint64_t        : variant offset      |
 *   uint16_t, str,  : variant id          |
 *   uint16_t, str,  : variant rsid        |
 *   uint16_t, str,  : variant chrom       |
 *   uint32_t,       : genetic position    | Metadata block
 *   uint16_t,       : number of alleles   |
 *   [                                     |
 *     uint32_t, str : allele id           |
 *   ], ...                                |
 * ], ...                                  /
 * [ uint32_t : number of partitions ],
 * [                                       \
 *   [                                     |
 *     uint64_t : partition offset         | Offsets block
 *   ], ...                                |
 * ]                                       /
 *
 * Version 03.
 */
#ifndef BGEN_META_H
#define BGEN_META_H

#include "bgen/file.h"
#include "bgen/str.h"
#include "bgen_export.h"

/** Metafile header: name field. */
#define BGEN_METAFILE_HDR_NAME "bgen index "
/** Metafile header: version field. */
#define BGEN_METAFILE_HDR_VERSION "03"
/** Metafile header: total length. */
#define BGEN_METAFILE_HDR_LENGTH 13

struct bgen_file;
/** Metafile handler.
 * @struct bgen_mf
 */
struct bgen_mf;

/** Variant metadata.
 * @struct bgen_vm
 */
struct bgen_vm
{
    long                    vaddr;      /**< Variante address. */
    struct bgen_str const*  id;         /**< Variant identification. */
    struct bgen_str const*  rsid;       /**< RSID. */
    struct bgen_str const*  chrom;      /**< Chromossome name. */
    uint32_t                position;   /**< Base-pair position. */
    uint16_t                nalleles;   /**< Number of alleles. */
    struct bgen_str const** allele_ids; /**< Allele ids. */
};

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
BGEN_EXPORT struct bgen_mf* bgen_metafile_create(struct bgen_file* bgen, char const* filepath,
                                                 unsigned npartitions, int verbose);
/** Open a bgen metafile.
 *
 * @param filepath File path to the metafile.
 * @return Metafile handler. `NULL` on failure.
 */
BGEN_EXPORT struct bgen_mf* bgen_open_metafile(char const* filepath);
/** Get the number of partitions.
 *
 * @param mf Metafile handler.
 * @return Number of partitions.
 */
BGEN_EXPORT unsigned bgen_metafile_npartitions(struct bgen_mf const* mf);
/** Get the number of variants.
 *
 * @param mf Metafile handler.
 * @return Number of variants.
 */
BGEN_EXPORT unsigned bgen_metafile_nvariants(struct bgen_mf const* mf);
/** Read a partition of variants.
 *
 * @param mf Metafile handler.
 * @param index Partition index.
 * @param nvariants Number of variants of the partition.
 * @return Array of variant metadata.
 */
BGEN_EXPORT struct bgen_vm* bgen_read_partition(struct bgen_mf const* mf, uint32_t index,
                                                uint32_t* nvariants);
/** Free a partition.
 *
 * @param vm Array of variant metatada.
 * @param nvariants Number of variants of the partition.
 */
BGEN_EXPORT void bgen_free_partition(struct bgen_vm* vm, uint32_t nvariants);
/** Close a metafile handler.
 *
 * @param mf Metafile handler.
 * @return `0` on success; `1` otherwise.
 */
BGEN_EXPORT int bgen_mf_close(struct bgen_mf* mf);

#endif
