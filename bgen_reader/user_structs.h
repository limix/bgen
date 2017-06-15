#ifndef USER_STRUCTS_H
#define USER_STRUCTS_H

#include "types.h"

typedef struct Variant
{
    string  id;
    string  rsid;
    string  chrom;
    inti    position;
    inti    nalleles;
    string *allele_ids;
} Variant;

typedef struct VariantGenotype
{
    inti  ploidy;
    real *probabilities;
} VariantGenotype;

typedef struct VariantIndexing
{
    char *filename;
    inti  compression;
    inti  layout;
    inti *start;
} VariantIndexing;

#endif /* end of include guard: USER_STRUCTS_H */
