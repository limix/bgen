/** Handy string definition.
 * @file: bgen/str.h
 */
#ifndef BGEN_STR_H
#define BGEN_STR_H

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

static inline bool bgen_str_equal(struct bgen_str a, struct bgen_str b)
{
    if (a.len == b.len)
        return strncmp(a.str, b.str, a.len) == 0;
    return 0;
}

static inline struct bgen_str BGEN_STR(char const* str)
{
    return (struct bgen_str){strlen(str), str};
}

/** String.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:type:`bgen_str` instead.
 * \endrst
 */
struct bgen_string
{
    int len;   /**< String length. */
    char* str; /**< Array of characters. */
} BGEN_DEPRECATED;

#endif
