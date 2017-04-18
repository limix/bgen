#include "bgen_reader/bgen_reader.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main()
{
    char *fp = "test/data/example.1bits.bgen";
    BGenFile bgen_file;
    int64_t  err = bgen_reader_read(&bgen_file, fp);

    if (err) return -1;

    assert(bgen_reader_nsamples(&bgen_file) == 500);
    assert(bgen_reader_nvariants(&bgen_file) == 199);

    printf("Number of samples: %lld\n",  bgen_reader_nsamples(&bgen_file));
    printf("Number of variants: %lld\n", bgen_reader_nvariants(&bgen_file));

    char *sample_id;
    uint64_t sample_id_length;
    uint64_t idx;

    idx = 0;

    if (bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length)) return -1;

    printf("Sample id for %llu-th: %.*s\n", idx, (int)sample_id_length,
           sample_id);

    idx = 499;

    if (bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length)) return -1;

    printf("Sample id for %llu-th: %.*s\n", idx, (int)sample_id_length,
           sample_id);


    idx = 500;
    assert(bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length) == -1);

    return 0;
}
