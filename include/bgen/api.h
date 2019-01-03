#ifndef BGEN_API_H
#define BGEN_API_H

#ifdef _WIN32
#ifdef BGEN_API_EXPORTS
#define BGEN_API __declspec(dllexport)
#else
#define BGEN_API __declspec(dllimport)
#endif
#else
#define BGEN_API
#endif

/* Borrowed from GLIB. */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#define BGEN_DEPRECATED __attribute__((__deprecated__)) BGEN_API
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define BGEN_DEPRECATED __declspec(deprecated) BGEN_API
#else
#define BGEN_DEPRECATED BGEN_API
#endif

#endif /* BGEN_API_H */
