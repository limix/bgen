#ifndef _BGEN_PBAR_H
#define _BGEN_PBAR_H

#include "athr.h"
#include "file.h"
#include <stddef.h>

static inline struct athr *create_athr(int n, const char *subject)
{
    struct athr *at = NULL;
    at = athr_create(n, subject, ATHR_BAR | ATHR_ETA);
    if (!at) {
        error("Could not create a progress bar");
        return NULL;
    }
    return at;
}

#endif /* _BGEN_PBAR_H */
