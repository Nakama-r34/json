#ifndef strings_h
#define strings_h

#include <stdio.h>
#include <string.h>

#include "types.h"

#define str_lit(s) string_create(s, strlen(s))

typedef struct {
    s8 *str;
    u64 size;
} string;

string string_create(s8 *c_string, u64 size);
string string_get_file(string path);
b8 is_num(u8 chr);
b8 is_alpha(u8 chr);

#endif