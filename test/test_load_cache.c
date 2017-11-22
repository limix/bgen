#include "example_files.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

int store_index(const byte *, const byte *);

int test_load(const byte *fp0, const byte *fp1) {
    struct BGenFile *bgen;
    struct BGenVar *variants;
    struct BGenVI *index;

    bgen = bgen_open(fp0);
    if (bgen == NULL)
        return FAIL;

    variants = bgen_load_variants(bgen, fp1, &index);
    if (variants == NULL)
        return FAIL;

    bgen_free_variants(bgen, variants);
    bgen_free_index(index);

    bgen_close(bgen);

    return EXIT_SUCCESS;
}

int main() {
    inti nexamples, prec, i;
    const byte *ex, *ix;

    nexamples = get_nexamples();

    for (i = 0; i < nexamples; ++i) {
        ex = get_example(i);
        ix = get_example_index(i);
        prec = get_example_precision(i);

        store_index(ex, ix);
        if (test_load(ex, ix) == FAIL)
            return FAIL;
    }

    return SUCCESS;
}
