#ifndef BGEN_UTIL_BITS_H
#define BGEN_UTIL_BITS_H

// Returns true / false if bit is set
#define GetBit(var, bit) ((var & (1 << bit)) != 0)
#define SetBit(var, bit) (var |= (1 << bit))

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

#define MIN(x, y)                                                              \
    ({                                                                         \
        typeof(x) _min1 = (x);                                                 \
        typeof(y) _min2 = (y);                                                 \
        (void)(&_min1 == &_min2);                                              \
        _min1 < _min2 ? _min1 : _min2;                                         \
    })

#define CEILDIV(x, y) ((x + (y - 1)) / y)

#endif /* end of include guard: BGEN_UTIL_BITS_H */
