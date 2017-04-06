#include <stdio.h>

void bgen_reader_read()
{
  char* fn = "/Users/horta/workspace/bgen-reader/test/example.1bits.bgen";
  FILE* f = fopen(fn, "r");

  fclose(f);
}
