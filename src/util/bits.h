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

#endif /* end of include guard: BGEN_UTIL_BITS_H */
