#ifndef BGEN_STDINT_H
#define BGEN_STDINT_H

#ifdef _MSC_VER
#if (_MSC_VER <= 1500)
#include "bgen/paul_stdint.h"
#else
#include <stdint.h>
#endif
#else
#include <stdint.h>
#endif

#endif /* end of include guard: BGEN_STDINT_H */
