#include "bgen.h"
#include <stdio.h>
#include <string.h>

int main() {
    struct bgen_file *bgen;
    struct bgen_vi **index;
    struct bgen_var *v;

    if ((bgen = bgen_open("data/example.14bits.bgen")) == NULL) {
        return 1;
    }

    v = bgen_load_variants_metadata(bgen, "data/wrong.metadata", index, 0);
    bgen_close(bgen);
    if (v != NULL)
        return 1;

    return 0;
}
