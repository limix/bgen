#ifndef BGEN_STRING_H
#define BGEN_STRING_H

void bgen_string_alloc(string *v, inti len);
inti bgen_fread_string2(FILE *file, string *s);
inti bgen_fread_string4(FILE *file, string *s);

#endif /* end of include guard: BGEN_STRING_H */
