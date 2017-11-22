#ifndef BGEN_UTIL_STDINT_H
#define BGEN_UTIL_STDINT_H

#ifdef _MSC_VER
#if (_MSC_VER <= 1500)
#include "util/paul_stdint.h"
#else
#include <stdint.h>
#endif
#else
#include <stdint.h>
#endif

#endif /* end of include guard: BGEN_UTIL_STDINT_H */
