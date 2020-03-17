#ifndef BGEN_IO_H
#define BGEN_IO_H

#include <stdint.h>
#include <stdio.h>

int     bgen_fseek(FILE* stream, int64_t offset, int origin);
int64_t bgen_ftell(FILE* stream);

#endif
