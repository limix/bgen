#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int64_t check_fread(size_t err, FILE* f, char* filepath)
{
  if (err != 1)
  {
          if (feof(f))
          {
                  fprintf(stderr, "Error reading %s: unexpected end of file.\n", filepath);
                  return -1;
          }
          fprintf(stderr, "Unknown error while reading %s.\n", filepath);
          return -1;
  }
  return 0;
}

int64_t bgen_reader_read()
{
        char* filepath = "/Users/horta/workspace/bgen-reader/test/example.1bits.bgen";
        FILE* f = fopen(filepath, "rb");

        uint32_t offset;

        if (check_fread(fread(&offset, 4, 1, f), f, filepath))
          return -1;


        printf("header_length: %u\n", offset);

        uint32_t header_length;
        if (check_fread(fread(&header_length, 4, 1, f), f, filepath))
          return -1;

        printf("header_length: %u\n", header_length);

        fclose(f);
        return 0;
}
