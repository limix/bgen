#include "bgen/bgen.h"
#include "cass.h"

void test_pbgen_example(void);

int main(void)
{
    test_pbgen_example();
    return cass_status();
}

void test_pbgen_example(void)
{
    struct bgen_file* bgen = bgen_file_open("data/pbgen_example.pgen");
    cass_cond(bgen == NULL);
}
