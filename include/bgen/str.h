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
    size_t len;      /**< String length. */
    char const* str; /**< Array of characters. */
};

BGEN_EXPORT static inline bool bgen_str_equal(struct bgen_str a, struct bgen_str b)
{
    if (a.len == b.len)
        return strncmp(a.str, b.str, a.len) == 0;
    return 0;
}

BGEN_EXPORT static inline struct bgen_str BGEN_STR(char const* str)
{
    return (struct bgen_str){strlen(str), str};
}

#endif
