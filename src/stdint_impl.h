#ifndef STDINT_IMPL_H
#define STDINT_IMPL_H

#ifdef _MSC_VER
# if (_MSC_VER <= 1500)
#  include "stdint_paul.h"
# else
#  include <stdint.h>
# endif
#else
# include <stdint.h>
#endif

#endif /* end of include guard: STDINT_IMPL_H */
