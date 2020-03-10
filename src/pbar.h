#ifndef BGEN_PBAR_H
#define BGEN_PBAR_H

#include "athr.h"
#include "file.h"
#include "report.h"
#include <stddef.h>

static inline struct athr *create_athr(long n, const char *subject)
{
    struct athr *at = NULL;
    at = athr_create(n, subject, ATHR_BAR | ATHR_ETA);
    if (!at) {
        bgen_error("Could not create a progress bar");
        return NULL;
    }
    return at;
}

#endif
