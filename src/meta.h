/*
 * This module reads and writes the bgen metafile, for index version 03.
 *
 * A bgen metafile is defined as follows.
 *
 * [ header block of 13 characters ], : "bgen index 03"
 * [ uint32_t : number of variants ],
 * [ uint64_t : meta information size ],
 * [
 *   uint64_t        : variant offset
 *   uint16_t, str,  : variant id
 *   uint16_t, str,  : variant rsid
 *   uint16_t, str,  : variant chrom
 *   uint32_t,       : genetic position
 *   uint16_t,       : number of alleles
 *   [
 *     uint16_t, str : allele id
 *   ], ...
 * ], ...
 * [ uint32_t : number of partitions ],
 * [
 *   [
 *     uint64_t : partition offset
 *   ], ...
 * ]
 */
#ifndef _BGEN_META_H
#define _BGEN_META_H

#define BGEN_IDX_NAME "bgen index "
#define BGEN_IDX_VER "03"
#define BGEN_HDR_LEN 13

#endif /* _BGEN_META_H */
