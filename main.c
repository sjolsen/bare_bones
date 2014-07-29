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
		vga_color oldcolor = vga_current_color;
		vga_setcolor (make_vga_color (COLOR_WHITE, COLOR_BLACK));
		vga_putchar (c);
		vga_setcolor (oldcolor);
	}
	else
		vga_putchar ('.');
}

static inline
void data_dump (uint32_t base, uint8_t (*get_data) (void* closure, uint32_t index), void* closure, uint32_t amount)
{
	char buffer [12];
	for (uint32_t row = 0; row < (amount + 15) / 16; ++row) {
		uint8_t data [16];
		uint8_t* dataptr = data;

		for (int col = 0; col < 16 && row * 16 + col < amount; ++col)
			*dataptr++ = get_data (closure, row * 16 + col);

		vga_puts (format_uint32_t (buffer, base + row * 16, 8, 16));
		vga_puts (": ");

		for (uint8_t* ptr = data; ptr != dataptr; ++ptr) {
			vga_puts (format_uint32_t (buffer, *ptr, 2, 16));
			if ((ptr - data) % 2 != 0)
				vga_putchar (' ');
		}
		vga_current_column = 51;

		for (uint8_t* ptr = data; ptr != dataptr; ++ptr)
			sanitarily_print_char (*ptr);

		vga_putchar ('\n');
	}
}

static inline
uint8_t ddump_csd (void* _ __attribute__ ((unused)), uint32_t index)
{ return chipset_data (index); }

static inline
uint8_t array_access (void* array, uint32_t index)
{ return ((uint8_t*) array) [index]; }

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
	char test [] = "This is some test string.";
	data_dump ((uintptr_t) test, &array_access, test, sizeof (test));

	vga_putline ("Chipset data:");
	data_dump (0, &ddump_csd, NULL, 256);

	vga_putline ("System halt");
}
