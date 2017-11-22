#include "example_files.h"

const char *examples[] = {"test/data/example.1bits.bgen",
                          "test/data/example.14bits.bgen",
                          "test/data/example.32bits.bgen"};

const char *indices[] = {"test/data/example.1bits.bgen.index",
                         "test/data/example.14bits.bgen.index",
                         "test/data/example.32bits.bgen.index"};

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
