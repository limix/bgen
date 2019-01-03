#ifndef BGEN_VARIANTS_INDEX_H
#define BGEN_VARIANTS_INDEX_H

#include "file.h"
#include "index.h"
#include <stdint.h>
#include <stdio.h>

#define BGEN_INDEX_NAME "bgen index "
#define BGEN_INDEX_VERSION "02"
#define BGEN_HEADER_LENGTH 13

struct bgen_vi *new_variants_index(const struct bgen_file *);

#endif /* BGEN_VARIANTS_INDEX_H */
