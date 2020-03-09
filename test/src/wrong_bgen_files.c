#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    cass_cond(bgen_file_open("") == NULL);
    cass_cond(bgen_file_open("nexist") == NULL);
    cass_cond(bgen_file_open("random.bgen") == NULL);
    cass_cond(bgen_file_open("example.14bits.bgen.truncated") == NULL);
    return cass_status();
}
