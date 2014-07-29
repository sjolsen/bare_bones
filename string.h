#ifndef STRING_H
#define STRING_H

static inline
size_t strlen (const char* str)
{
	size_t length = 0;
	while (str [length] != '\0')
		++length;
	return length;
}

#endif
