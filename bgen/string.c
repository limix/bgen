#include <stdio.h>
#include "types.h"
#include "file.h"
#include "code.h"

void bgen_string_alloc(string *v, inti len)
{
    v->len = len;
    v->str = malloc(len + 1);
    v->str[len] = '\0';
}

inti bgen_fread_string2(FILE *file, string *s)
{
    uint16_t len;

    if (bgen_read(file, &len, 2) == FAIL)
        return FAIL;

    bgen_string_alloc(s, len);

    if (bgen_read(file, s->str, s->len) == FAIL)
        return FAIL;
    return SUCCESS;
}

inti bgen_fread_string4(FILE *file, string *s)
{
    uint32_t len;

    if (bgen_read(file, &len, 4) == FAIL)
        return FAIL;

    bgen_string_alloc(s, len);

    if (bgen_read(file, s->str, s->len) == FAIL)
        return FAIL;
    return SUCCESS;
}
