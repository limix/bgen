#ifndef BGEN_NUMBER_H
#define BGEN_NUMBER_H

#include "bgen/stdint.h"

#ifdef WIN32
#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *)__nan)
#endif
#endif

#ifdef _MSC_VER
#if (_MSC_VER <= 1500)
#define isnan(x) _isnan(x)
#endif
#endif

typedef char byte;
typedef int_fast64_t inti;
typedef double real;

#endif /* end of include guard: BGEN_NUMBER_H */
