#include "bgen.h"
#include "cass.h"
#include <stdio.h>
#include <string.h>

int main()
{
    struct bgen_file *bgen;
    struct bgen_vi *index = NULL;
    struct bgen_var *v;

    if ((bgen = bgen_open("data/example.14bits.bgen")) == NULL) {
        return 1;
    }

    v = bgen_load_variants_metadata(bgen, "data/wrong.metadata", &index, 0);
    bgen_close(bgen);
    assert_null(v);

    return 0;
}
