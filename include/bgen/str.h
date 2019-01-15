/** Handy string definition.
 * @file: bgen/str.h
 */
#ifndef BGEN_STR_H
#define BGEN_STR_H

#include "bgen/api.h"

/** String.
 * @struct bgen_str
 */
struct bgen_str
{
    int len;   /**< String length. */
    char *str; /**< Array of characters. */
};

/** String.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:type:`bgen_str` instead.
 * \endrst
 */
struct bgen_string
{
    int len;   /**< String length. */
    char *str; /**< Array of characters. */
} BGEN_DEPRECATED;

#endif /* BGEN_STR_H */
