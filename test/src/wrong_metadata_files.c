#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    cass_cond(bgen_metafile_open("") == NULL);
    cass_cond(bgen_metafile_open("nexist") == NULL);
    cass_cond(bgen_metafile_open("random.bgen.metadata") == NULL);
    cass_cond(bgen_metafile_open("example.14bits.bgen.metadata.truncated") == NULL);
    return cass_status();
}
