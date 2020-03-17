/* Define low-level io read and write functions. */
#ifndef BGEN_IO_H
#define BGEN_IO_H

#include <stdint.h>
#include <stdio.h>

int     bgen_fseek(FILE* stream, int64_t offset, int origin);
int64_t bgen_ftell(FILE* stream);

/* Define fwrite_int, fwrite_ui64 and like. */
#define DECLARE_TYPE_FWRITE(TYPE, SUF) int fwrite_##SUF(FILE*, TYPE, size_t);
DECLARE_TYPE_FWRITE(uint64_t, ui64)
DECLARE_TYPE_FWRITE(uint32_t, ui32)
DECLARE_TYPE_FWRITE(uint16_t, ui16)
DECLARE_TYPE_FWRITE(int64_t, i64)
DECLARE_TYPE_FWRITE(int32_t, i32)
DECLARE_TYPE_FWRITE(int16_t, i16)
#undef DECLARE_TYPE_FWRITE

#endif
