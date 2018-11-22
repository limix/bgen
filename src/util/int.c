#include "util/int.h"

#define DEFINE_TYPE_FREAD(TYPE)                                                         \
    int TYPE##_fread(FILE *fp, TYPE *value, size_t size) {                              \
        *value = 0;                                                                     \
                                                                                        \
        if (fread(value, size, 1, fp) != 1) {                                           \
                                                                                        \
            if (feof(fp))                                                               \
                fprintf(stderr, "Error reading file: unexpected end of file.\n");       \
            else if (ferror(fp))                                                        \
                perror("Error reading file");                                           \
                                                                                        \
            return 1;                                                                   \
        }                                                                               \
                                                                                        \
        return 0;                                                                       \
    }

DEFINE_TYPE_FREAD(int)
DEFINE_TYPE_FREAD(unsigned)
DEFINE_TYPE_FREAD(uint64_t)
DEFINE_TYPE_FREAD(uint32_t)
DEFINE_TYPE_FREAD(uint16_t)

#define DEFINE_TYPE_FWRITE(TYPE)                                                        \
    int TYPE##_fwrite(FILE *fp, TYPE value, size_t size) {                              \
                                                                                        \
        if (fwrite(&value, size, 1, fp) != 1) {                                         \
                                                                                        \
            if (feof(fp))                                                               \
                fprintf(stderr, "Error writing file: unexpected end of file.\n");       \
            else if (ferror(fp))                                                        \
                perror("Error writing file");                                           \
                                                                                        \
            return 1;                                                                   \
        }                                                                               \
        return 0;                                                                       \
    }

DEFINE_TYPE_FWRITE(int)
DEFINE_TYPE_FWRITE(unsigned)
DEFINE_TYPE_FWRITE(uint64_t)
DEFINE_TYPE_FWRITE(uint32_t)
DEFINE_TYPE_FWRITE(uint16_t)

#undef DEFINE_TYPE_FREAD
#undef DEFINE_TYPE_FWRITE
