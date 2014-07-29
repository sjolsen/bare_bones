#include "format.h"

static inline
char digit_to_ascii (uint_fast8_t digit)
{
	if (digit < 10)
		return digit + '0';
	else
		return (digit - 10) + 'A';
}

static inline
void creverse (char* begin, char* end)
{
	if (begin == end)
		return;
	--end;

	while (begin < end) {
		char tmp = *begin;
		*begin = *end;
		*end = tmp;
		++begin;
		--end;
	}
}


// Extern functions

char* format_int (char* buffer, int32_t value, uint_fast8_t mincol, uint_fast8_t base)
{
	if (value < 0) {
		char* numstring = format_uint (buffer + 1, -value, mincol, base);
		--numstring;
		numstring [0] = '-';
		return numstring;
	}
	else
		return format_uint (buffer + 1, value, mincol, base);
}

char* format_uint (char* buffer, uint32_t value, uint_fast8_t mincol, uint_fast8_t base)
{
	char* cursor = buffer;

	do {
		*cursor = digit_to_ascii (value % base);
		value /= base;
		++cursor;
	}
	while (value != 0);

	while (cursor < buffer + mincol)
		*cursor++ = '0';
	creverse (buffer, cursor);
	*cursor = '\0';

	return buffer;
}
