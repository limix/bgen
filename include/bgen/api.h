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
#define BGEN_DEPR __attribute__((__deprecated__))
#else
#define BGEN_DEPR
#endif

#define BGEN_DEPRECATED BGEN_DEPR
#define BGEN_DEPRECATED_API BGEN_DEPR BGEN_API

#endif /* BGEN_API_H */
