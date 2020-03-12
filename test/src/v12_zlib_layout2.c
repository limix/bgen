#include "bgen/bgen.h"
#include "cass.h"
#include <stdio.h>
#include <math.h>

const char* get_example_filepath(size_t i);
const char* get_example_index_filepath(size_t i);
unsigned    get_example_precision(size_t i);
unsigned    get_nexamples(void);
unsigned    ipow(unsigned base, unsigned exp);
void        test_read_metadata(struct bgen_file* bgen, struct bgen_samples* samples,
                               struct bgen_metafile* metafile);
void        test_read_probabilities(struct bgen_file* bgen, struct bgen_metafile* metafile,
                                    uint32_t nsamples, unsigned prec);
void test_read(struct bgen_file* bgen, struct bgen_metafile* metafile, unsigned precision);

int main(void)
{
    for (unsigned i = 0; i < get_nexamples(); ++i) {
        const char* ex = get_example_filepath(i);
        const char* ix = get_example_index_filepath(i);
        unsigned    prec = get_example_precision(i);

        struct bgen_file*     bgen = bgen_file_open(ex);
        struct bgen_metafile* mf = bgen_metafile_open(ix);

        test_read(bgen, mf, prec);

        cass_cond(bgen_metafile_close(mf) == 0);
        bgen_file_close(bgen);
    }

    return cass_status();
}

unsigned ipow(unsigned base, unsigned exp)
{
    unsigned result = 1;
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

void test_read_metadata(struct bgen_file* bgen, struct bgen_samples* samples,
                        struct bgen_metafile* metafile)
{
    cass_cond(bgen_file_nsamples(bgen) == 500);
    cass_cond(bgen_file_nvariants(bgen) == 199);
    cass_cond(bgen_str_equal(BGEN_STR("sample_001"), *bgen_samples_get(samples, 0)));
    cass_cond(bgen_str_equal(BGEN_STR("sample_500"), *bgen_samples_get(samples, 499)));

    struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, 0);
    cass_cond(bgen_partition_nvariants(partition) == 67);
    cass_cond(bgen_str_equal(BGEN_STR("SNPID_2"), *bgen_partition_get(partition, 0)->id));
    bgen_partition_destroy(partition);

    partition = bgen_metafile_read_partition(metafile, 1);
    cass_cond(bgen_partition_nvariants(partition) == 67);
    cass_cond(bgen_str_equal(BGEN_STR("SNPID_74"), *bgen_partition_get(partition, 5)->id));
    bgen_partition_destroy(partition);

    partition = bgen_metafile_read_partition(metafile, 2);
    cass_cond(bgen_partition_nvariants(partition) == 65);
    cass_cond(bgen_str_equal(BGEN_STR("SNPID_196"), *bgen_partition_get(partition, 60)->id));
    bgen_partition_destroy(partition);
}

void test_read_probabilities(struct bgen_file* bgen, struct bgen_metafile* metafile,
                             uint32_t nsamples, unsigned prec)
{
    double prob[3];
    double abs_tol = 1. / ipow(2, prec) + 1. / ipow(2, prec) / 3.;
    double rel_tol = 1e-09;
    int    e;
    size_t i, j;

    FILE* f = fopen("data/example.matrix", "r");
    cass_cond(f != NULL);

    uint32_t ii = 0;
    i = 0;
    for (uint32_t part = 0; part < bgen_metafile_npartitions(metafile); ++part) {
        struct bgen_partition const* partition = bgen_metafile_read_partition(metafile, part);
        for (ii = 0; ii < bgen_partition_nvariants(partition); ++ii, ++i) {
            struct bgen_variant const* vm = bgen_partition_get(partition, ii);
            struct bgen_genotype*      vg = bgen_file_open_genotype(bgen, vm->genotype_offset);

            cass_cond(bgen_genotype_max_ploidy(vg) == 2);

            unsigned ncombs = bgen_genotype_ncombs(vg);
            cass_cond(ncombs == 3);

            double* probabilities = calloc(nsamples * ncombs, sizeof(double));

            cass_cond(bgen_genotype_read(vg, probabilities) == 0);

            for (j = 0; j < 500; ++j) {

                fprintf(stderr, "Ponto 1\n");
                e = fscanf(f, "%lf", prob + 0);
                if (e != 1)
                {
                    perror("fscanf has failed?");
                }
                fprintf(stderr, "Ponto 1.e: %d\n", e);
                if (feof(f))
                    fprintf(stderr, "End of file\n");
                else
                    fprintf(stderr, "NOT end of file\n");
                fprintf(stderr, "READ: %lf\n CORRECT: %lf", prob[0], probabilities[j * 3 + 0]);
                fflush(stderr);
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
        bgen_partition_destroy(partition);
    }

    fclose(f);
}

void test_read(struct bgen_file* bgen, struct bgen_metafile* metafile, unsigned precision)
{
    struct bgen_samples* samples = bgen_file_read_samples(bgen, 0);
    test_read_metadata(bgen, samples, metafile);
    bgen_samples_free(samples);

    test_read_probabilities(bgen, metafile, 500, precision);
}

const char* examples[] = {"data/example.1bits.bgen", "data/example.14bits.bgen",
                          "data/example.32bits.bgen"};

const char* indices[] = {"data/example.1bits.bgen.metadata",
                         "data/example.14bits.bgen.metadata",
                         "data/example.32bits.bgen.metadata"};

const unsigned precision[] = {1, 14, 32};

const char* get_example_filepath(size_t i) { return examples[i]; }

const char* get_example_index_filepath(size_t i) { return indices[i]; }

unsigned get_example_precision(size_t i) { return precision[i]; }

unsigned get_nexamples() { return 3; }
