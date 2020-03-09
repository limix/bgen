#include "index.h"
#include "mem.h"

int init_index(struct bgen_file const* bgen, struct bgen_vi* vi)
{
    vi->filepath = strdup(bgen_file_filepath(bgen));
    vi->compression = (uint32_t)bgen_file_compression(bgen);
    vi->layout = (uint32_t)bgen_file_layout(bgen);
    vi->nsamples = (uint32_t)bgen_file_nsamples(bgen);
    vi->nvariants = (uint32_t)bgen_file_nvariants(bgen);
    vi->max_nalleles = 0;
    return 0;
}
