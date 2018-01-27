#ifndef BITS_H
#define BITS_H

#define GetBit(var, bit) ((var & (1 << bit)) != 0) // Returns true / false if bit is set
#define SetBit(var, bit) (var |= (1 << bit))

#ifdef WIN32
# ifndef NAN
   static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#  define NAN (*(const float *) __nan)
# endif
#endif

#endif /* end of include guard: BITS_H */
