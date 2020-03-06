#include "bgen.h"
#include "cass.h"
#include <stdio.h>
#include <string.h>

int main()
{
    struct bgen_file *bgen;
    struct bgen_vi *index = NULL;
    struct bgen_var *v;

    assert_not_null(bgen = bgen_open("data/example.14bits.bgen"));

    v = bgen_load_variants_metadata(bgen, "data/wrong.metadata", &index, 0);
    assert_null(v);

    bgen_close(bgen);

    return 0;
}
