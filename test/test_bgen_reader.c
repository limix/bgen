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

    if (bgen_reader_layout(&bgen_file) != 1) return -1;

    if (bgen_reader_compression(&bgen_file) != 2) return -1;

    if (bgen_reader_nsamples(&bgen_file) != 500) return -1;

    if (bgen_reader_nvariants(&bgen_file) != 199) return -1;

    char *sample_id;
    uint64_t sample_id_length;
    uint64_t idx;

    idx = 0;

    if (bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length)) return -1;

    if (strncmp(sample_id, "sample_001", sample_id_length) != 0) return -1;

    idx = 499;

    if (bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length)) return -1;

    if (strncmp(sample_id, "sample_500", sample_id_length) != 0) return -1;

    idx = 500;
    assert(bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                                 &sample_id_length) == -1);

    return 0;
}
