#include "format.h"

static inline
char digit_to_ascii (uint_fast8_t digit)
{
	return digit + '0';
}


// Extern functions

char* format_int32_t (char buffer [static 12], int32_t value)
{
	if (value < 0) {
		char* numstring = format_uint32_t (buffer + 1, -value);
		--numstring;
		numstring [0] = '-';
		return numstring;
	}
	else
		return format_uint32_t (buffer + 1, value);
}

char* format_uint32_t (char buffer [static 11], uint32_t value)
{
	char* numstring = buffer + 10;
	numstring [0] = '\0';

	do {
		--numstring;
		*numstring = digit_to_ascii (value % 10);
		value /= 10;
	}
	while (value != 0);

	return numstring;
}
