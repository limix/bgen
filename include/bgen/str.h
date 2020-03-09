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

BGEN_EXPORT struct bgen_str const* bgen_str_create(char const* data, size_t length);
BGEN_EXPORT void bgen_str_free(struct bgen_str const* bgen_str);

BGEN_EXPORT static inline char const* bgen_str_data(struct bgen_str const* bgen_str)
{
    return bgen_str->data;
}

BGEN_EXPORT static inline size_t bgen_str_length(struct bgen_str const* bgen_str)
{
    return bgen_str->length;
}

BGEN_EXPORT static inline bool bgen_str_equal(struct bgen_str a, struct bgen_str b)
{
    if (a.length == b.length)
        return strncmp(a.data, b.data, a.length) == 0;
    return 0;
}

BGEN_EXPORT static inline struct bgen_str BGEN_STR(char const* str)
{
    return (struct bgen_str){strlen(str), str};
}

#endif
