#include "io.h"
#include "report.h"
#include <limits.h>

/* Avoid fseek()'s 2GiB barrier with MSVC, macOS, *BSD, MinGW */
/* Credits to ZSTD library */
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define LONG_SEEK _fseeki64
#define LONG_TELL _ftelli64
#elif !defined(__64BIT__) &&                                                                  \
    (PLATFORM_POSIX_VERSION >= 200112L) /* No point defining Large file for 64 bit */
#define LONG_SEEK fseeko
#define LONG_TELL ftello
#elif defined(__MINGW32__) && !defined(__STRICT_ANSI__) && !defined(__NO_MINGW_LFS) &&        \
    defined(__MSVCRT__)
#define LONG_SEEK fseeko64
#define LONG_TELL ftello64
#elif defined(_WIN32) && !defined(__DJGPP__)
#include <windows.h>
static int LONG_SEEK(FILE* file, __int64 offset, int origin)
{
    LARGE_INTEGER off;
    DWORD         method;
    off.QuadPart = offset;
    if (origin == SEEK_END)
        method = FILE_END;
    else if (origin == SEEK_CUR)
        method = FILE_CURRENT;
    else
        method = FILE_BEGIN;

    if (SetFilePointerEx((HANDLE)_get_osfhandle(_fileno(file)), off, NULL, method))
        return 0;
    else
        return -1;
}
static __int64 LONG_TELL(FILE* file)
{
    LARGE_INTEGER off, newOff;
    off.QuadPart = 0;
    newOff.QuadPart = 0;
    SetFilePointerEx((HANDLE)_get_osfhandle(_fileno(file)), off, &newOff, FILE_CURRENT);
    return newOff.QuadPart;
}
#else
#define LONG_SEEK fseek
#define LONG_TELL ftell
#endif

int bgen_fseek(FILE* stream, int64_t offset, int origin)
{
    return LONG_SEEK(stream, offset, origin);
}

int64_t bgen_ftell(FILE* stream) { return LONG_TELL(stream); }
