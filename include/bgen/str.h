/** Handy string definition.
 * @file: str.h
 */
#ifndef BGEN_STR_H
#define BGEN_STR_H

#include "bgen/api.h"

/** String. */
struct bgen_str
{
    int len;   /**< String length. */
    char *str; /**< Array of characters. */
};

/** String. */
struct bgen_string
{
    int len;   /**< String length. */
    char *str; /**< Array of characters. */
} BGEN_DEPRECATED;

#endif /* BGEN_STR_H */
