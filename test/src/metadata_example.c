#include "bgen/bgen.h"
#include "cass.h"

void test_corrupted_metafile_file(void);

int main(void)
{
    test_corrupted_metafile_file();
    return cass_status();
}

void test_corrupted_metafile_file(void)
{
    cass_cond(bgen_metafile_open(TEST_DATADIR "wrong.metafile") == NULL);
}
