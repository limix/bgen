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

    if (err) return EXIT_FAILURE;

    if (bgen_reader_layout(&bgen_file) != 1) return EXIT_FAILURE;

    if (bgen_reader_compression(&bgen_file) != 2) return EXIT_FAILURE;

    if (bgen_reader_nsamples(&bgen_file) != 500) return EXIT_FAILURE;

    if (bgen_reader_nvariants(&bgen_file) != 199) return EXIT_FAILURE;

    if (bgen_reader_sample_identifiers(&bgen_file) != 1) return EXIT_FAILURE;

    char *sample_id, *variant_id, *variant_rsid, *variant_chrom;
    uint64_t sample_id_length, variant_id_length, variant_rsid_length,
             variant_chrom_length;
    uint64_t idx;

    idx = 0;

    if (bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length)) return EXIT_FAILURE;

    if (strncmp(sample_id, "sample_001",
                sample_id_length) != 0) return EXIT_FAILURE;

    idx = 499;

    if (bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length)) return EXIT_FAILURE;

    if (strncmp(sample_id, "sample_500",
                sample_id_length) != 0) return EXIT_FAILURE;

    idx = 500;

    if (bgen_reader_sample_id(&bgen_file, idx, &sample_id,
                              &sample_id_length) !=
    EXIT_FAILURE) return EXIT_FAILURE;

    idx = 0;

    bgen_reader_variant_id(&bgen_file, idx, &variant_id,
                           &variant_id_length);

    if (strncmp(variant_id, "SNPID_2", variant_id_length) != 0) return EXIT_FAILURE;


    bgen_reader_variant_rsid(&bgen_file, idx, &variant_rsid, &variant_rsid_length);

    if (strncmp(variant_rsid, "RSID_2", variant_rsid_length) != 0) return EXIT_FAILURE;

    bgen_reader_variant_chrom(&bgen_file, idx, &variant_chrom, &variant_chrom_length);

    if (strncmp(variant_chrom, "01", variant_chrom_length) != 0) return EXIT_FAILURE;

    if (bgen_reader_variant_position(&bgen_file, idx) != 2000) return EXIT_FAILURE;

    if (bgen_reader_variant_nalleles(&bgen_file, idx) != 2) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
