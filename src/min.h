#ifndef _MIN_H
#define _MIN_H

#include <stddef.h>

inline static int imin(int a, int b) { return a < b ? a : b; }
inline static size_t smin(size_t a, size_t b) { return a < b ? a : b; }

#endif /* _MIN_H */
