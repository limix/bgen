#ifndef BGEN_TEST_EXAMPLE_FILES_H
#define BGEN_TEST_EXAMPLE_FILES_H

#include <stddef.h>

const char *get_example_filepath(size_t i);
const char *get_example_index_filepath(size_t i);
int get_example_precision(size_t i);
int get_nexamples();

#endif /* BGEN_TEST_EXAMPLE_FILES_H */
