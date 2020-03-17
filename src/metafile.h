/** Create and query a metafile.
 * @file bgen/metafile.h
 *
 * A bgen metafile is defined as follows:
 *
 * [ char[13] : signature (e.g. "bgen index 04") ], \
 * [ uint32_t : number of variants ],               | Header block
 * [ uint32_t : number of partitions ],             |
 * [ uint64_t : metadata block size ],              /
 * [                                                \
 *   [                                              |
 *     uint64_t : partition offset (this file)      | Offsets block
 *   ], ...                                         |
 * ]                                                /
 * [                                                \
 *   uint64_t        : genotype offset (bgen file)  |
 *   uint16_t, str,  : variant id                   |
 *   uint16_t, str,  : variant rsid                 |
 *   uint16_t, str,  : variant chrom                |
 *   uint32_t,       : genetic position             | Metadata block
 *   uint16_t,       : number of alleles            |
 *   [                                              |
 *     uint32_t, str : allele id                    |
 *   ], ...                                         |
 * ], ...                                           /
 *
 * Version 04.
 */
#ifndef BGEN_METAFILE_H_PRIVATE
#define BGEN_METAFILE_H_PRIVATE

#include <inttypes.h>
#include <stdio.h>

#define BGEN_METAFILE_SIGNATURE "bgen index 04"
#define BGEN_METAFILE_HEADER_SIZE (13 + 4 + 4 + 8)

struct bgen_metafile
{
    char*     filepath;
    FILE*     stream;
    uint32_t  nvariants;
    uint32_t  npartitions;
    uint64_t  metadata_block_size;
    uint64_t* partition_offset; /**< Array of partition offsets */
};

uint32_t bgen_metafile_partition_size(uint32_t nvariants, uint32_t npartitions);

#endif
