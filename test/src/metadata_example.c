#include "bgen/bgen.h"
#include "cass.h"
#include <stdio.h>
#include <string.h>

void test_corrupted_metadata_file(void);

int main()
{
    test_corrupted_metadata_file();
    return cass_status();
}

void test_corrupted_metadata_file(void)
{
    cass_cond(bgen_open_metafile("data/wrong.metadata") == NULL);
}
