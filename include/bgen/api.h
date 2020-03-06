#ifndef BGEN_API_H
#define BGEN_API_H

#ifdef _WIN32
#ifdef BGEN_EXPORTS
#define BGEN_API __declspec(dllexport)
#else
#define BGEN_API __declspec(dllimport)
#endif
#else
#define BGEN_API __attribute__((visibility("default")))
#endif

#endif
