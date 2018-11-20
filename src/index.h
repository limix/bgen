#ifndef BGEN_VARIANTS_INDEX3_H
#define BGEN_VARIANTS_INDEX3_H

#include "bgen.h"
#include <stdio.h>
#include <stdlib.h>

#define BGEN_IDX_NAME "bgen index "
#define BGEN_IDX_VER "03"
#define BGEN_HDR_LEN 13

struct bgen_cmf;

struct bgen_cmf *bgen_create_metafile_open(const char *filepath, uint32_t nvariants,
                                           uint32_t npartitions);

int bgen_create_metafile_write_loop(struct bgen_cmf *cmf,
                                    struct bgen_var *(*next)(uint64_t *, void *),
                                    void *cb_args, int verbose);

int bgen_create_metafile_close(struct bgen_cmf *cmf);

#endif /* end of include guard: BGEN_VARIANTS_INDEX3_H */
