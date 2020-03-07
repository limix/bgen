#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    cass_cond(bgen_open_metafile("") == NULL);
    cass_cond(bgen_open_metafile("nexist") == NULL);
    cass_cond(bgen_open_metafile("random.bgen.metadata") == NULL);
    cass_cond(bgen_open_metafile("example.14bits.bgen.metadata.truncated") == NULL);
    return cass_status();
}
