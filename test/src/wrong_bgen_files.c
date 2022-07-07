#include "bgen/bgen.h"
#include "cass.h"

int main(void)
{
    cass_cond(bgen_file_open("") == NULL);
    cass_cond(bgen_file_open(TEST_DATADIR "nexist") == NULL);
    cass_cond(bgen_file_open(TEST_DATADIR "random.bgen") == NULL);
    cass_cond(bgen_file_open(TEST_DATADIR "example.14bits.bgen.truncated") == NULL);
    return cass_status();
}
