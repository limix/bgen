#include "example_files.h"

const char *examples[] = {"data/example.1bits.bgen",
                          "data/example.14bits.bgen",
                          "data/example.32bits.bgen"};

const char *indices[] = {"data/example.1bits.bgen.index",
                         "data/example.14bits.bgen.index",
                         "data/example.32bits.bgen.index"};

const int precision[] = {1, 14, 32};

const char *get_example_filepath(int i)
{
    return examples[i];
}

const char *get_example_index_filepath(int i)
{
    return indices[i];
}

int get_example_precision(int i)
{
    return precision[i];
}

int get_nexamples()
{
    return 3;
}
