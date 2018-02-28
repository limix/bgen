#include "example_files.h"

#include "bgen.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

#ifndef isnan
int isnan(double x) {
#if defined _MSC_VER
  union {
    __int64 u;
    double f;
  } ieee754;
#else
  union {
    uint64_t u;
    double f;
  } ieee754;
#endif
  ieee754.f = x;
  return ((unsigned)(ieee754.u >> 32) & 0x7fffffff) +
             ((unsigned)ieee754.u != 0) >
         0x7ff00000;
}
#endif

int test_read_metadata(struct BGenFile *bgen, bgen_string *s,
                       struct BGenVar *v) {
  if (bgen_nsamples(bgen) != 500)
    return 1;

  if (bgen_nvariants(bgen) != 199)
    return 1;

  if (strncmp("sample_001", s[0].str, s[0].len) != 0)
    return 1;

  if (strncmp("sample_500", s[499].str, s[0].len) != 0)
    return 1;

  if (strncmp("SNPID_2", v[0].id.str, v[0].id.len) != 0)
    return 1;

  if (strncmp("SNPID_200", v[198].id.str, v[198].id.len) != 0)
    return 1;

  return 0;
}

int test_reading(const char *fp0, const char *fp1, struct BGenVI **index) {
  struct BGenFile *bgen;
  struct BGenVar *v;
  bgen_string *s;

  if ((bgen = bgen_open(fp0)) == NULL)
    return 1;

  s = bgen_read_samples(bgen, 0);
  if (fp1)
    v = bgen_load_variants(bgen, fp1, index, 0);
  else
    v = bgen_read_variants(bgen, index, 0);

  if (test_read_metadata(bgen, s, v))
    return 1;

  bgen_free_samples(bgen, s);
  bgen_free_variants(bgen, v);

  bgen_close(bgen);

  return 0;
}

int test_read_probabilities(struct BGenVI *index, int nsamples, int prec) {
  struct BGenVG *vg;
  FILE *f;
  double prob[3];
  double eps = (double)(1 << prec);
  int ncombs;
  size_t i, j;
  double *probabilities;

  if ((f = fopen("data/example.matrix", "r")) == NULL)
    return 1;

  for (i = 0; i < 199; ++i) {
    vg = bgen_open_variant_genotype(index, i);

    if (bgen_ploidy(vg) != 2) {
      fprintf(stderr, "Wrong ploidy.\n");
      return 1;
    }

    ncombs = bgen_ncombs(vg);

    probabilities = calloc(nsamples * ncombs, sizeof(double));

    bgen_read_variant_genotype(index, vg, probabilities);

    for (j = 0; j < 500; ++j) {
      fscanf(f, "%lf", prob + 0);
      fscanf(f, "%lf", prob + 1);
      fscanf(f, "%lf", prob + 2);

      if ((prob[0] == 0) && (prob[1] == 0) && (prob[2] == 0)) {
        prob[0] = NAN;
        prob[1] = NAN;
        prob[2] = NAN;

        if (!isnan(probabilities[j * 3 + 0]))
          return 1;

        if (!isnan(probabilities[j * 3 + 1]))
          return 1;

        if (!isnan(probabilities[j * 3 + 2]))
          return 1;
      } else {
        if (fabs(prob[0] - probabilities[j * 3 + 0]) > eps)
          return 1;

        if (fabs(prob[1] - probabilities[j * 3 + 1]) > eps)
          return 1;

        if (fabs(prob[2] - probabilities[j * 3 + 2]) > eps)
          return 1;
      }
    }
    bgen_close_variant_genotype(index, vg);
    free(probabilities);
  }

  fclose(f);

  return 0;
}

int test_read(const char *bgen_fp, const char *index_fp, int precision) {
  struct BGenVI *index;

  if (test_reading(bgen_fp, index_fp, &index))
    return 1;

  if (test_read_probabilities(index, 500, precision))
    return 1;

  bgen_free_index(index);

  return 0;
}

int main() {

  size_t i;
  int prec;
  const char *ex, *ix;

  for (i = 0; i < (size_t)get_nexamples(); ++i) {
    ex = get_example_filepath(i);
    ix = get_example_index_filepath(i);
    prec = get_example_precision(i);

    bgen_create_variants_index_file(ex, ix, 0);

    if (test_read(ex, NULL, prec))
      return 1;

    if (test_read(ex, ix, prec))
      return 1;
  }

  return 0;
}