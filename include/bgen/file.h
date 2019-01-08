/*
 * Open, close, and query bgen file.
 */
#ifndef BGEN_FILE_H
#define BGEN_FILE_H

#include "api.h"
#include "str.h"

struct bgen_file; /* bgen file handler */

/* Open a file and return a bgen file handler. */
BGEN_API struct bgen_file *bgen_open(const char *filepath);
/* Close a bgen file handler. */
BGEN_API void bgen_close(struct bgen_file *bgen);
/* Get the number of samples. */
BGEN_API int bgen_nsamples(const struct bgen_file *bgen);
/* Get the number of variants. */
BGEN_API int bgen_nvariants(const struct bgen_file *bgen);
/* Check if the file contain sample identifications. */
BGEN_API int bgen_contain_samples(const struct bgen_file *bgen);
/* Get array of sample identifications. */
BGEN_API struct bgen_str *bgen_read_samples(struct bgen_file *bgen, int verbose);
/* Free array of sample identifications. */
BGEN_API void bgen_free_samples(const struct bgen_file *, struct bgen_str *);

#endif /* BGEN_FILE_H */
