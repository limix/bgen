#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    cass_cond(bgen_metafile_open("") == NULL);
    cass_cond(bgen_metafile_open("data/nexist") == NULL);
    cass_cond(bgen_metafile_open("data/random.bgen.metafile") == NULL);
    cass_cond(bgen_metafile_open("data/example.14bits.bgen.metafile.truncated") == NULL);
    return cass_status();
}
