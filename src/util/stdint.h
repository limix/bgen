#ifndef STDINT_H
#define STDINT_H

#ifdef _MSC_VER
#if (_MSC_VER <= 1500)
#include "util/paul_stdint.h"
#else
#include <stdint.h>
#endif
#else
#include <stdint.h>
#endif

#endif /* end of include guard: STDINT_H */
