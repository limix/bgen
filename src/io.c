#include "io.h"
#include "report.h"
#include <limits.h>

// Avoid fseek()'s 2GiB barrier with MSVC, macOS, *BSD, MinGW
// Credits to ZSTD library
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define LONG_SEEK _fseeki64
#define LONG_TELL _ftelli64
typedef __int64 OFF_T;
#define OFF_T_MAX +9223372036854775807 /* this is a guess */
#define OFF_T_MIN -9223372036854775807 /* this is a guess */
#elif !defined(__64BIT__) &&                                                                  \
    (PLATFORM_POSIX_VERSION >= 200112L) /* No point defining Large file for 64 bit */
#define LONG_SEEK fseeko
#define LONG_TELL ftello
typedef off_t OFF_T;
#define OFF_T_MAX +9223372036854775807 /* this is a guess */
#define OFF_T_MIN -9223372036854775807 /* this is a guess */
#elif defined(__MINGW32__) && !defined(__STRICT_ANSI__) && !defined(__NO_MINGW_LFS) &&        \
    defined(__MSVCRT__)
#define LONG_SEEK fseeko64
#define LONG_TELL ftello64
typedef off64_t OFF_T;
#define OFF_T_MAX +9223372036854775807 /* this is a guess */
#define OFF_T_MIN -9223372036854775807 /* this is a guess */
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
