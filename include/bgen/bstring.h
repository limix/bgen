/** Handy string definition.
 * @file: bgen/bstring.h
 */
#ifndef BGEN_BSTRING_H
#define BGEN_BSTRING_H

#include "bgen/export.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/** String.
 * @struct bgen_string
 */
struct bgen_string
{
    size_t      length; /**< String length. */
    char const* data;   /**< Array of characters. */
};

/** Create a bgen string.
 *
 * @param data Usual C string. It does not need to be null-terminated.
 * @param length String length.
 * @return Bgen string.
 */
static inline struct bgen_string const* bgen_string_create(char const* data, size_t length)
{
    struct bgen_string* str = (struct bgen_string*)malloc(sizeof(struct bgen_string));
    str->data = data;
    str->length = length;
    return str;
}
/** Destroy a bgen string.
 *
 * @param bgen_string Bgen string.
 */
static inline void bgen_string_destroy(struct bgen_string const* bgen_string)
{
    if (bgen_string->length > 0)
        free((char*)bgen_string->data);
    free((struct bgen_string*)bgen_string);
}
/** Get a pointer to the C string.
 *
 * @param bgen_string Bgen string.
 * @return Pointer to the internal C string.
 */
static inline char const* bgen_string_data(struct bgen_string const* bgen_string)
{
    return bgen_string->data;
}
/** Get string length.
 *
 * @param bgen_string Bgen string.
 * @return Length.
 */
static inline size_t bgen_string_length(struct bgen_string const* bgen_string)
{
    return bgen_string->length;
}
/** Compare if two bgen strings are equal.
 *
 * @param a First bgen string.
 * @param b Second bgen string.
 * @return `true` if they are equal; `false` otherwise.
 */
static inline bool bgen_string_equal(struct bgen_string a, struct bgen_string b)
{
    if (a.length == b.length)
        return strncmp(a.data, b.data, a.length) == 0;
    return 0;
}
/** Initialize a bgen string from a null-terminated string.
 *
 * @param str Null-terminated string.
 * @return Bgen string.
 */
static inline struct bgen_string BGEN_STRING(char const* str)
{
    return (struct bgen_string){strlen(str), str};
}

#endif
