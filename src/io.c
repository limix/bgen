#include "io.h"

#define DEFINE_TYPE_FREAD(TYPE, SUF)                                                   \
    int fread_##SUF(FILE *fp, TYPE *value, size_t size)                                \
    {                                                                                  \
        *value = 0;                                                                    \
                                                                                       \
        if (fread(value, size, 1, fp) != 1) {                                          \
                                                                                       \
            if (feof(fp))                                                              \
                fprintf(stderr, "Error reading file: unexpected end of file.\n");      \
            else if (ferror(fp))                                                       \
                perror("Error reading file");                                          \
                                                                                       \
            return 1;                                                                  \
        }                                                                              \
                                                                                       \
        return 0;                                                                      \
    }

DEFINE_TYPE_FREAD(int, int)
DEFINE_TYPE_FREAD(unsigned, unsigned)
DEFINE_TYPE_FREAD(uint64_t, ui64)
DEFINE_TYPE_FREAD(uint32_t, ui32)
DEFINE_TYPE_FREAD(uint16_t, ui16)

#define DEFINE_TYPE_FWRITE(TYPE, SUF)                                                  \
    int fwrite_##SUF(FILE *fp, TYPE value, size_t size)                                \
    {                                                                                  \
                                                                                       \
        if (fwrite(&value, size, 1, fp) != 1) {                                        \
                                                                                       \
            if (feof(fp))                                                              \
                fprintf(stderr, "Error writing file: unexpected end of file.\n");      \
            else if (ferror(fp))                                                       \
                perror("Error writing file");                                          \
                                                                                       \
            return 1;                                                                  \
        }                                                                              \
        return 0;                                                                      \
    }

DEFINE_TYPE_FWRITE(int, int)
DEFINE_TYPE_FWRITE(unsigned, unsigned)
DEFINE_TYPE_FWRITE(uint64_t, ui64)
DEFINE_TYPE_FWRITE(uint32_t, ui32)
DEFINE_TYPE_FWRITE(uint16_t, ui16)