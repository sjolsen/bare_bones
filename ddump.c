#include "ddump.h"
#include "vga.h"
#include "format.h"

static inline
void sanitarily_print_char (char c)
{
	if (' ' <= c && c < 127) {
		vga_color oldcolor = vga_getcolor ();
		vga_setcolor (make_vga_color (COLOR_WHITE, oldcolor.bg));
		vga_putchar (c);
		vga_setcolor (oldcolor);
	}
	else
		vga_putchar ('.');
}

static
ptrdiff_t data_dump_row (uint32_t base, const uint8_t* begin, const uint8_t* end)
{
	enum {
		DDCOLS = 16
	};

	char print_buffer [12];
	if (begin + 16 < end)
		end = begin + 16;

	vga_puts (format_uint (print_buffer, base, 8, 16));
	vga_putchar (':');

	for (const uint8_t* ptr = begin; ptr != end; ++ptr) {
		if ((ptr - begin) % 2 == 0)
			vga_putchar (' ');
		vga_puts (format_uint (print_buffer, *ptr, 2, 16));
	}

	// End of column is ((col * 5 + 1) / 2)
	int spaces = ((DDCOLS * 5 + 1) / 2) - (((end - begin) * 5 + 1) / 2);
	for (int i = 0; i < spaces; ++i)
		vga_putchar (' ');
	vga_puts ("  ");

	for (const uint8_t* ptr = begin; ptr != end; ++ptr)
		sanitarily_print_char (*ptr);

	vga_putchar ('\n');
	return end - begin;
}


// Extern functions

void data_dump (uint32_t base, const void* begin, const void* end)
{
	const uint8_t* _begin = (const uint8_t*) begin;
	const uint8_t* _end   = (const uint8_t*) end;

	while (_begin != _end) {
		ptrdiff_t increment = data_dump_row (base, _begin, _end);
		base   += increment;
		_begin += increment;
	}
}
