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

#endif /* BGEN_API_H */
