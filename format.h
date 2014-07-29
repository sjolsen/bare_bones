#ifndef FORMAT_H
#define FORMAT_H

#include <stdint.h>

char* format_int (char* buffer, int32_t value, uint_fast8_t mincol, uint_fast8_t base);
char* format_uint (char* buffer, uint32_t value, uint_fast8_t mincol, uint_fast8_t base);

#endif
