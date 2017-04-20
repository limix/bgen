#ifndef BITS_H
#define BITS_H

#define GetBit(var, bit) ((var & (1 << bit)) != 0) // Returns true / false if bit is set
#define SetBit(var, bit) (var |= (1 << bit))
#define FlipBit(var, bit) (var ^= (1 << bit))

#endif /* end of include guard: BITS_H */
