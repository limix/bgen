#include "example_files.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

int store_index(const byte *, const byte *);

int test_store(const byte *bgenfp, const byte *cachefp) {
    return store_index(bgenfp, cachefp);
}

int main() {
    inti nexamples, prec, i;
    const byte *ex, *ix;

    nexamples = get_nexamples();

    for (i = 0; i < nexamples; ++i) {
        ex = get_example(i);
        ix = get_example_index(i);
        prec = get_example_precision(i);

        if (test_store(ex, ix))
            return 1;
    }
    return 0;
}
