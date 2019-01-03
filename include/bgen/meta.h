/* Create and query a metafile, for index version 03.
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
 *     uint32_t, str : allele id
 *   ], ...
 * ], ...
 * [ uint32_t : number of partitions ],
 * [
 *   [
 *     uint64_t : partition offset
 *   ], ...
 * ]
 */
#ifndef BGEN_META_H
#define BGEN_META_H

#include "api.h"
#include "file.h"
#include "str.h"

#define BGEN_IDX_NAME "bgen index "
#define BGEN_IDX_VER "03"
#define BGEN_HDR_LEN 13

struct bgen_mf; /* metafile */

/* Variant metadata. */
struct bgen_vm
{
    long vaddr; /* variant offset-address */
    struct bgen_str id;
    struct bgen_str rsid;
    struct bgen_str chrom;
    int position; /* base-pair position */
    int nalleles; /* number of alleles */
    struct bgen_str *allele_ids;
};

BGEN_API struct bgen_mf *bgen_create_metafile(struct bgen_file *, const char *, int,
                                              int);
BGEN_API struct bgen_mf *bgen_open_metafile(const char *filepath);
BGEN_API int bgen_metafile_nparts(struct bgen_mf *);
BGEN_API int bgen_metafile_nvars(struct bgen_mf *);
BGEN_API struct bgen_vm *bgen_read_partition(struct bgen_mf *, int, int *);
BGEN_API void bgen_free_partition(struct bgen_vm *, int);
BGEN_API int bgen_close_metafile(struct bgen_mf *);
#if BGEN_OPEN_GENOTYPE == 3
/* Open a variant for genotype queries. */
BGEN_API struct bgen_vg *bgen_open_genotype(struct bgen_vm *);
#endif

#endif /* BGEN_META_H */
