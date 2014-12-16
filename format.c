#include "format.h"
#include "string.h"

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

static inline
void divide_by_zero (void)
{
	__asm__ ("int $0x00"::);
}

static inline
void divmod (uint64_t (*quotient), uint64_t (*remainder),
             uint64_t numerator, uint64_t denominator)
{
	if (denominator == 0)
		divide_by_zero ();

	uint64_t denscale = 1;
	while (denominator < numerator) {
		denominator <<= 1;
		denscale <<= 1;
	}

	uint64_t _quotient = 0;
	while (denscale != 0) {
		if (denominator <= numerator) {
			numerator -= denominator;
			_quotient += denscale;
		}
		denominator >>= 1;
		denscale >>= 1;
	}

	(*quotient) = _quotient;
	(*remainder) = numerator;
}


// Extern functions

char* numsep (char* bufstr, char sep)
{
	size_t n = strlen (bufstr);
	size_t m = n + (n - 1)/3;
	char* src = bufstr + n - 1;
	char* dst = bufstr + m - 1;
	for (int i = 0; src != dst;) {
		if (i == 3) {
			i = 0;
			*dst = sep;
		}
		else {
			i = i + 1;
			*dst = *src;
			--src;
		}
		--dst;
	}
	bufstr [m] = '\0';
	return bufstr;
}

char* format_int (char* buffer, int64_t value, uint_fast8_t mincol, uint_fast8_t base)
{
	if (value < 0) {
		char* numstring = format_uint (buffer + 1, -value, mincol, base);
		--numstring;
		numstring [0] = '-';
		return numstring;
	}
	else
		return format_uint (buffer, value, mincol, base);
}

char* format_uint (char* buffer, uint64_t value, uint_fast8_t mincol, uint_fast8_t base)
{
	char* cursor = buffer;

	do {
		uint64_t div, mod;
		divmod (&div, &mod, value, base);
		*cursor = digit_to_ascii (mod);
		value = div;
		++cursor;
	}
	while (value != 0);

	while (cursor < buffer + mincol)
		*cursor++ = '0';
	creverse (buffer, cursor);
	*cursor = '\0';

	return buffer;
}
