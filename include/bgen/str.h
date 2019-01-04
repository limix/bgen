#ifndef BGEN_STR_H
#define BGEN_STR_H

#include "bgen/api.h"

struct bgen_str
{
    int len;
    char *str;
};

struct bgen_string
{
    int len;
    char *str;
} BGEN_DEPRECATED;

#endif /* BGEN_STR_H */
