#include "example_files.h"

const byte *examples[] = {"test/data/example.1bits.bgen",
                          "test/data/example.14bits.bgen",
                          "test/data/example.32bits.bgen"};

const byte *indices[] = {"test/data/example.1bits.bgen.index",
                         "test/data/example.14bits.bgen.index",
                         "test/data/example.32bits.bgen.index"};

const inti precision[] = {1, 14, 32};

const byte *get_example(inti i)
{
    return examples[i];
}

const byte *get_example_index(inti i)
{
    return indices[i];
}

inti get_example_precision(inti i)
{
    return precision[i];
}

inti get_nexamples()
{
    return 3;
}

