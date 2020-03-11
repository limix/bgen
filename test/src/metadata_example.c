#include "bgen/bgen.h"
#include "cass.h"

void test_corrupted_metadata_file(void);

int main()
{
    test_corrupted_metadata_file();
    return cass_status();
}

void test_corrupted_metadata_file(void)
{
    cass_cond(bgen_metafile_open("data/wrong.metadata") == NULL);
}
