#include "io.h"
#include "report.h"
#include <limits.h>

// Avoid fseek()'s 2GiB barrier with MSVC, macOS, *BSD, MinGW
// Credits to ZSTD library
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define LONG_SEEK _fseeki64
#define LONG_TELL _ftelli64
typedef __int64 OFF_T;
#define OFF_T_MAX 9223372036854775807 /* this is a guess */
#define OFF_T_MIN 9223372036854775808 /* this is a guess */
#elif !defined(__64BIT__) &&                                                                  \
    (PLATFORM_POSIX_VERSION >= 200112L) /* No point defining Large file for 64 bit */
#define LONG_SEEK fseeko
#define LONG_TELL ftello
typedef off_t OFF_T;
#define OFF_T_MAX 9223372036854775807 /* this is a guess */
#define OFF_T_MIN 9223372036854775808 /* this is a guess */
#elif defined(__MINGW32__) && !defined(__STRICT_ANSI__) && !defined(__NO_MINGW_LFS) &&        \
    defined(__MSVCRT__)
#define LONG_SEEK fseeko64
#define LONG_TELL ftello64
typedef off64_t OFF_T;
#define OFF_T_MAX 9223372036854775807 /* this is a guess */
#define OFF_T_MIN 9223372036854775808 /* this is a guess */
#elif defined(_WIN32) && !defined(__DJGPP__)
static_assert(0, "Code does not work on the DJGPP compiler.");
#else
#define LONG_SEEK fseek
#define LONG_TELL ftell
typedef long OFF_T;
#define OFF_T_MAX LONG_MAX
#define OFF_T_MIN LONG_MIN
#endif

int bgen_fseek(FILE* stream, int64_t offset, int origin)
{
    if (offset > OFF_T_MAX)
        bgen_die("fseek overflow");

    if (offset < OFF_T_MIN)
        bgen_die("fseek underflow");

    return LONG_SEEK(stream, offset, origin);
}

int64_t bgen_ftell(FILE* stream) { return LONG_TELL(stream); }

#define DEFINE_TYPE_FREAD(TYPE, SUF)                                                          \
    int fread_##SUF(FILE* fp, TYPE* value, size_t size)                                       \
    {                                                                                         \
        *value = 0;                                                                           \
                                                                                              \
        if (fread(value, size, 1, fp) != 1) {                                                 \
                                                                                              \
            if (feof(fp))                                                                     \
                bgen_error("error reading file (unexpected end of file)");                    \
            else                                                                              \
                bgen_perror("error reading file");                                            \
                                                                                              \
            return 1;                                                                         \
        }                                                                                     \
                                                                                              \
        return 0;                                                                             \
    }

DEFINE_TYPE_FREAD(int, int)
DEFINE_TYPE_FREAD(long, long)
DEFINE_TYPE_FREAD(unsigned, unsigned)
DEFINE_TYPE_FREAD(uint64_t, ui64)
DEFINE_TYPE_FREAD(uint32_t, ui32)
DEFINE_TYPE_FREAD(uint16_t, ui16)
DEFINE_TYPE_FREAD(int64_t, i64)
DEFINE_TYPE_FREAD(int32_t, i32)
DEFINE_TYPE_FREAD(int16_t, i16)

#define DEFINE_TYPE_FWRITE(TYPE, SUF)                                                         \
    int fwrite_##SUF(FILE* fp, TYPE value, size_t size)                                       \
    {                                                                                         \
        if (fwrite(&value, size, 1, fp) != 1) {                                               \
                                                                                              \
            if (feof(fp))                                                                     \
                bgen_error("error reading file (unexpected end of file)");                    \
            else                                                                              \
                bgen_perror("error writing file");                                            \
                                                                                              \
            return 1;                                                                         \
        }                                                                                     \
        return 0;                                                                             \
    }

DEFINE_TYPE_FWRITE(int, int)
DEFINE_TYPE_FWRITE(long, long)
DEFINE_TYPE_FWRITE(unsigned, unsigned)
DEFINE_TYPE_FWRITE(uint64_t, ui64)
DEFINE_TYPE_FWRITE(uint32_t, ui32)
DEFINE_TYPE_FWRITE(uint16_t, ui16)
DEFINE_TYPE_FWRITE(int64_t, i64)
DEFINE_TYPE_FWRITE(int32_t, i32)
DEFINE_TYPE_FWRITE(int16_t, i16)
