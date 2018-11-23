/* This module reads and writes the bgen index version 03.
 *
 * A bgen index file is defined as follows.
 *
 * [ 13 chars : "bgen index 03"     ],
 * [ uint32_t : number of variants  ],
 * [ uint64_t : metadata block size ],
 * Metadata block
 *   [
 *     uint64_t        : variant offset in the bgen file
 *     uint16_t, str,  : variant id
 *     uint16_t, str,  : variant rsid
 *     uint16_t, str,  : variant chrom
 *     uint32_t,       : genetic position
 *     uint16_t,       : number of alleles
 *     [
 *       uint32_t, str : allele id
 *     ], ...
 *   ], ...
 * Meta-index block
 * [ uint32_t : npartitions ],
 * [ uint64_t : position of partition 0 ],
 * [ uint64_t : position of partition 1 ], ...
 */
#define BGEN_API_EXPORTS

#include "index.h"
#include "bits.h"
#include "endian.h"
#include "io.h"
#include "mem.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
