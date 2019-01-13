#define BGEN_API_EXPORTS

#include "file.h"
#include "bgen.h"

BGEN_DEPRECATED_API int bgen_sample_ids_presence(const struct bgen_file *bgen)
{
    return bgen->sample_ids_presence;
}
