#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

static inline
size_t strlen (const char* str)
{
	size_t length = 0;
	while (str [length] != '\0')
		++length;
	return length;
}

static inline
bool ascii_printable (char c)
{
	return ' ' <= c && c <= '~';
}

#endif
