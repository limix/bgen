#include "bgen_reader/bgen_reader.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main()
{
    BGenFile bgen_file;
    int64_t  err = bgen_reader_read(&bgen_file);

    if (err) return -1;

    printf("Number of samples: %lld\n", bgen_reader_nsamples(&bgen_file));
    printf("Number of variants: %lld\n", bgen_reader_nvariants(&bgen_file));
}
