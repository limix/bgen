/** Handy string definition.
 * @file: bgen/str.h
 */
#ifndef BGEN_STR_H
#define BGEN_STR_H

#include "bgen_export.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/** String.
 * @struct bgen_str
 */
struct bgen_str
{
    size_t length;    /**< String length. */
    char const* data; /**< Array of characters. */
};

/* BGEN_EXPORT void bgen_str_alloc(struct bgen_str* bgen_str, size_t length); */
BGEN_EXPORT struct bgen_str const* bgen_str_create(char const* data, size_t length);
BGEN_EXPORT char const* bgen_str_data(struct bgen_str const* bgen_str);
BGEN_EXPORT void bgen_str_free(struct bgen_str const* bgen_str);
BGEN_EXPORT size_t bgen_str_length(struct bgen_str const* bgen_str);
BGEN_EXPORT bool bgen_str_equal(struct bgen_str a, struct bgen_str b);
BGEN_EXPORT struct bgen_str BGEN_STR(char const* str);

#endif
