#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#ifdef _MSC_VER
# if (_MSC_VER <= 1500)
   typedef long long int_fast64_t;
# else
#  include <inttypes.h>
# endif
#else
# include <inttypes.h>
#endif
#include <assert.h>

typedef unsigned char byte;
typedef int_fast64_t  inti;
typedef double        real;

typedef struct string {
    inti  len;
    byte *str;
} string;


#endif /* end of include guard: TYPES_H */
