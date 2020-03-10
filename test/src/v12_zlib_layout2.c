#include "example_files.h"

#include "bgen/bgen.h"
#include "cass.h"
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* get_example_filepath(size_t i);
const char* get_example_index_filepath(size_t i);
int get_example_precision(size_t i);
int get_nexamples();

int ipow(int base, int exp)
{
    int result = 1;
    for (;;) {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

void test_read_metadata(struct bgen_file* bgen, struct bgen_samples *samples, struct bgen_mf* mf)
{
    cass_cond(bgen_file_nsamples(bgen) == 500);
    cass_cond(bgen_file_nvariants(bgen) == 199);
    cass_cond(bgen_str_equal(BGEN_STR("sample_001"), *bgen_samples_get(samples, 0)));
    cass_cond(bgen_str_equal(BGEN_STR("sample_500"), *bgen_samples_get(samples, 499)));

    int nvariants = 0;
    struct bgen_vm* vm = bgen_read_partition(mf, 0, &nvariants);
    cass_cond(nvariants == 67);
    cass_cond(bgen_str_equal(BGEN_STR("SNPID_2"), *vm[0].id));
    bgen_free_partition(vm, nvariants);

    vm = bgen_read_partition(mf, 1, &nvariants);
    cass_cond(nvariants == 67);
    cass_cond(bgen_str_equal(BGEN_STR("SNPID_74"), *vm[5].id));
    bgen_free_partition(vm, nvariants);

    vm = bgen_read_partition(mf, 2, &nvariants);
    cass_cond(nvariants == 65);
    cass_cond(bgen_str_equal(BGEN_STR("SNPID_196"), *vm[60].id));
    bgen_free_partition(vm, nvariants);
}

void test_read_probabilities(struct bgen_file* bgen, struct bgen_mf* mf, int nsamples,
                             int prec)
{
    double prob[3];
    double abs_tol = 1. / ipow(2, prec) + 1. / ipow(2, prec) / 3.;
    double rel_tol = 1e-09;
    int e;
    size_t i, j;

    FILE* f = fopen("data/example.matrix", "r");
    cass_cond(f != NULL);

    int nvariants = 0;
    int ii = 0;
    i = 0;
    for (int part = 0; part < bgen_metafile_npartitions(mf); ++part) {
        struct bgen_vm* vm = bgen_read_partition(mf, part, &nvariants);
        for (ii = 0; ii < nvariants; ++ii, ++i) {
            struct bgen_genotype* vg = bgen_file_open_genotype(bgen, vm[ii].genotype_offset);

            cass_cond(bgen_max_ploidy(vg) == 2);

            int ncombs = bgen_ncombs(vg);
            cass_cond(ncombs == 3);

            double* probabilities = calloc(nsamples * ncombs, sizeof(double));

            cass_cond(bgen_read_genotype(bgen, vg, probabilities) == 0);

            for (j = 0; j < 500; ++j) {

                e = fscanf(f, "%lf", prob + 0);
                cass_cond(e == 1);

                e = fscanf(f, "%lf", prob + 1);
                cass_cond(e == 1);

                e = fscanf(f, "%lf", prob + 2);
                cass_cond(e == 1);

                if ((prob[0] == 0) && (prob[1] == 0) && (prob[2] == 0)) {
                    prob[0] = NAN;
                    prob[1] = NAN;
                    prob[2] = NAN;
                    cass_cond(isnan(probabilities[j * 3 + 0]));
                    cass_cond(isnan(probabilities[j * 3 + 1]));
                    cass_cond(isnan(probabilities[j * 3 + 2]));
                } else {
                    cass_close2(probabilities[j * 3 + 0], prob[0], rel_tol, abs_tol);
                    cass_close2(probabilities[j * 3 + 1], prob[1], rel_tol, abs_tol);
                    cass_close2(probabilities[j * 3 + 2], prob[2], rel_tol, abs_tol);
                }
            }
            free(probabilities);
            bgen_genotype_close(vg);
        }
        bgen_free_partition(vm, nvariants);
    }

    fclose(f);
}

void test_read(struct bgen_file* bgen, struct bgen_mf* mf, int precision)
{
    struct bgen_samples* samples = bgen_file_read_samples(bgen, 0);
    test_read_metadata(bgen, samples, mf);
    bgen_samples_free(samples);

    test_read_probabilities(bgen, mf, 500, precision);
}

int main()
{
    size_t i;

    for (i = 0; i < (size_t)get_nexamples(); ++i) {
        const char* ex = get_example_filepath(i);
        const char* ix = get_example_index_filepath(i);
        int prec = get_example_precision(i);

        struct bgen_file* bgen = bgen_file_open(ex);
        struct bgen_mf* mf = bgen_open_metafile(ix);

        test_read(bgen, mf, prec);

        cass_cond(bgen_mf_close(mf) == 0);
        bgen_file_close(bgen);
    }

    return cass_status();
}

const char* examples[] = {"data/example.1bits.bgen", "data/example.14bits.bgen",
                          "data/example.32bits.bgen"};

const char* indices[] = {"data/example.1bits.bgen.metadata",
                         "data/example.14bits.bgen.metadata",
                         "data/example.32bits.bgen.metadata"};

const int precision[] = {1, 14, 32};

const char* get_example_filepath(size_t i) { return examples[i]; }

const char* get_example_index_filepath(size_t i) { return indices[i]; }

int get_example_precision(size_t i) { return precision[i]; }

int get_nexamples() { return 3; }
