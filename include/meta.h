/* Create and query a metafile.
 */
#ifndef BGEN_META_H
#define BGEN_META_H

#include "api.h"
#include "file.h"
#include "str.h"

struct bgen_mf; /* metafile */

/* Variant metadata. */
struct bgen_vm
{
    int vaddr; /* variant offset-address */
    struct bgen_str id;
    struct bgen_str rsid;
    struct bgen_str chrom;
    int position; /* base-pair position */
    int nalleles; /* number of alleles */
    struct bgen_str *allele_ids;
};

BGEN_API struct bgen_mf *bgen_create_metafile(struct bgen_file *, const char *, int);
BGEN_API struct bgen_mf *bgen_open_metafile(const char *filepath);
BGEN_API int bgen_metafile_nparts(struct bgen_mf *);
BGEN_API int bgen_metafile_nvars(struct bgen_mf *);
BGEN_API struct bgen_vm *bgen_read_partition(struct bgen_mf *, int, int *);
BGEN_API void bgen_free_partition(struct bgen_vm *, int);
BGEN_API void bgen_free_metadata(struct bgen_vm *);
BGEN_API int bgen_close_metafile(struct bgen_mf *);

#endif /* BGEN_META_H */
