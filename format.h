#ifndef FORMAT_H
#define FORMAT_H

#include <stdint.h>

char* format_int32_t (char buffer [static 12], int32_t value);
char* format_uint32_t (char buffer [static 11], uint32_t value);

#endif
