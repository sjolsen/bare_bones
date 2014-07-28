#include "vga.h"

#define ARRAYLEN(a) (sizeof (a) / sizeof (*a))

void kernel_main (void)
{
	vga_initialize ();

	vga_color_code colors [] = {
		COLOR_RED,
		COLOR_LIGHT_RED,
		COLOR_LIGHT_BROWN,
		COLOR_GREEN,
		COLOR_CYAN,
		COLOR_BLUE,
		COLOR_MAGENTA
	};

	for (size_t i = 0; i < ARRAYLEN (colors); ++i) {
		vga_setcolor (make_vga_color (colors [i], COLOR_BLACK));
		vga_puts ("Hello, kernel World! This is a very long line.");
	}

	vga_setcolor (make_vga_color (COLOR_WHITE, COLOR_BLACK));
	for (size_t i = 0; i < 512; ++i)
		vga_putchar ('A' + (i % (1 + 'Z' - 'A')));
}
