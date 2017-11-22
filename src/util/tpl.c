#include "util/tpl.h"
#include "bgen/bgen.h"
#include "tpl/tpl.h"

#include <stdlib.h>
#include <string.h>

inti tpl_append_buffer(tpl_node *tn, struct Buffer *b)
{
    byte *mem;
    size_t size;
    uint64_t size64;

    tpl_dump(tn, TPL_GETSIZE, &size);
    size64 = size;
    buffer_append(b, &size64, sizeof(size64));

    mem = malloc(size);
    tpl_dump(tn, TPL_MEM|TPL_PREALLOCD, mem, size);
    buffer_append(b, mem, size);
    free(mem);

    tpl_free(tn);
    return SUCCESS;
}
