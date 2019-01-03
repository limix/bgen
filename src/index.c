#include "index.h"
#include "mem.h"

int init_index(const struct bgen_file *bgen, struct bgen_vi *vi)
{
    vi->filepath = strdup(bgen->filepath);
    vi->compression = bgen->compression;
    vi->layout = bgen->layout;
    vi->nsamples = bgen->nsamples;
    vi->nvariants = bgen->nvariants;
    vi->max_nalleles = 0;
    /* `start` is not used anymore. */
    vi->start = NULL;
    return 0;
}
