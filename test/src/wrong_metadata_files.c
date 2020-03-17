#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    cass_cond(bgen_metafile_open_04("") == NULL);
    cass_cond(bgen_metafile_open_04("data/nexist") == NULL);
    cass_cond(bgen_metafile_open_04("data/random.bgen.metafile") == NULL);
    cass_cond(bgen_metafile_open_04("data/example.14bits.bgen.metafile.truncated") == NULL);
    return cass_status();
}
