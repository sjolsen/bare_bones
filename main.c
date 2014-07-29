#include "vga.h"
#include "format.h"
#include "portio.h"

static inline
uint32_t cr0 (void)
{
	uint32_t ret;
	__asm__ ("movl %%cr0, %0" : "=r" (ret));
	return ret;
}

static inline
uint8_t chipset_data (uint8_t index)
{
	outb (0x22, index);
	return inb (0x23);
}

static inline
void sanitarily_print_char (char c)
{
	if (' ' <= c && c < 127) {
		vga_color oldcolor = vga_getcolor ();
		vga_setcolor (make_vga_color (COLOR_WHITE, COLOR_BLACK));
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

	vga_puts (format_uint32_t (print_buffer, base, 8, 16));
	vga_putchar (':');

	for (const uint8_t* ptr = begin; ptr != end; ++ptr) {
		if ((ptr - begin) % 2 == 0)
			vga_putchar (' ');
		vga_puts (format_uint32_t (print_buffer, *ptr, 2, 16));
	}

	// End of column is ((col * 5 + 1) / 2)
	int spaces = ((DDCOLS * 5 + 1) / 2) - (((end - begin) * 5 + 1) / 2);
	for (int i = 0; i < spaces; ++i)
		vga_putchar (' ');
	vga_putchar (' ');

	for (const uint8_t* ptr = begin; ptr != end; ++ptr)
		sanitarily_print_char (*ptr);

	vga_putchar ('\n');
	return end - begin;
}

static inline
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

static inline
size_t strlen (const char* str)
{
	size_t length = 0;
	while (str [length] != '\0')
		++length;
	return length;
}

void kernel_main (void)
{
	vga_initialize ();

	char buffer [12];

	vga_puts ("cr0: ");
	vga_putline (format_uint32_t (buffer, cr0 (), 0, 10));

	vga_puts ("INT_MIN: ");
	vga_putline (format_int32_t (buffer, (int) 0x80000000, 0, 10));
	vga_puts ("INT_MIN: 0x");
	vga_putline (format_uint32_t (buffer, 0x80000000, 0, 16));

	vga_putline ("Testing data dumper:");
	const char* test1 = "This is some test string.";
	const char test2 [] = "This is another test string.";
	data_dump ((uintptr_t) test1, test1, test1 + strlen (test1) + 1);
	data_dump ((uintptr_t) test2, test2, test2 + sizeof (test2));

	vga_putline ("Chipset data:");
	uint8_t cdata [256];
	for (size_t i = 0; i < 256; ++i)
		cdata [i] = chipset_data (i);
	data_dump (0, cdata, cdata + 256);

	vga_putline ("System halt");
}
