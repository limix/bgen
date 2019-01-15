#include "index.h"
#include "mem.h"

int init_index(const struct bgen_file *bgen, struct bgen_vi *vi)
{
    vi->filepath = strdup(bgen->filepath);
    vi->compression = (uint32_t)bgen->compression;
    vi->layout = (uint32_t)bgen->layout;
    vi->nsamples = (uint32_t)bgen->nsamples;
    vi->nvariants = (uint32_t)bgen->nvariants;
    vi->max_nalleles = 0;
    /* `start` is not used anymore. */
    vi->start = NULL;
    return 0;
}
