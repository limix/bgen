/** Create and query a metafile.
 * @file bgen/meta.h
 *
 * A bgen metafile is defined as follows:
 *
 * [ char[13] : header (e.g. "bgen index 03") ],
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
#ifndef BGEN_METAFILE_H_PRIVATE
#define BGEN_METAFILE_H_PRIVATE

#include <inttypes.h>
#include <stdio.h>

/** Metafile header: name field. */
#define BGEN_METAFILE_HDR_NAME "bgen index "
/** Metafile header: version field. */
#define BGEN_METAFILE_HDR_VERSION "03"
/** Metafile header: total length. */
#define BGEN_METAFILE_HDR_LENGTH 13

struct bgen_mf
{
    char*     filepath;
    FILE*     stream;
    uint32_t  nvariants;
    uint64_t  metadata_size; /**< Size of the metadata block */
    uint32_t  npartitions;
    uint64_t* partition_offset; /**< Array of partition offsets */
};

#endif
