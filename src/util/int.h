#ifndef INT_H
#define INT_H

#include <stdio.h>
#include <stdlib.h>

#define DECLARE_TYPE_FREAD(TYPE) int TYPE##_fread(FILE *, TYPE *, size_t);

DECLARE_TYPE_FREAD(int);
DECLARE_TYPE_FREAD(unsigned);
DECLARE_TYPE_FREAD(uint64_t);
DECLARE_TYPE_FREAD(uint32_t);
DECLARE_TYPE_FREAD(uint16_t);

#define DECLARE_TYPE_FWRITE(TYPE) int TYPE##_fwrite(FILE *, TYPE, size_t);

DECLARE_TYPE_FWRITE(int);
DECLARE_TYPE_FWRITE(unsigned);
DECLARE_TYPE_FWRITE(uint64_t);
DECLARE_TYPE_FWRITE(uint32_t);
DECLARE_TYPE_FWRITE(uint16_t);

#undef DECLARE_TYPE_FREAD
#undef DECLARE_TYPE_FWRITE

#endif
