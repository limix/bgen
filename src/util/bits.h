#ifndef BGEN_UTIL_BITS_H
#define BGEN_UTIL_BITS_H

// Returns true / false if bit is set
#define GetBit(var, bit) ((var & (1 << bit)) != 0)
#define SetBit(var, bit) (var |= (1 << bit))

#ifndef MIN
#define MIN(x, y) x < y ? x : y;
#endif

#define CEILDIV(x, y) ((x + (y - 1)) / y)

#endif /* end of include guard: BGEN_UTIL_BITS_H */
