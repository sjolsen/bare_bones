#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "IRQ.h"
#include "keyboard.h"
#include "format.h"
#include "PC16550D.h"
#include "portio.h"

#define ARRAYLEN(a) (sizeof (a) / sizeof (*a))

void ISR_PIT (__attribute__ ((unused)) INT_index interrupt)
{
	static const char sdata [] = "Hello, Rainbow Dance Party OS!\n";
	static const size_t slength = ARRAYLEN (sdata) - 1;
	static size_t scursor = 0;

	static const vga_color_code cdata [] = {
		COLOR_LIGHT_RED,
		COLOR_RED,
		COLOR_BROWN,
		COLOR_LIGHT_BROWN,
		COLOR_LIGHT_GREEN,
		COLOR_GREEN,
		COLOR_CYAN,
		COLOR_LIGHT_BLUE,
		COLOR_BLUE,
		COLOR_MAGENTA,
		COLOR_LIGHT_MAGENTA
	};
	static const size_t clength = ARRAYLEN (cdata);
	static size_t ccursor = 0;

	vga_setcolor (make_vga_color (cdata [ccursor], COLOR_BLACK));
	vga_putchar (sdata [scursor]);
	if (++scursor == slength) {
		scursor = 0;
		if (++ccursor == clength)
			ccursor = 0;
	}
}

void kernel_main (/* multiboot_info_t* info, uint32_t magic */)
{
	vga_initialize ();
	GDT_initialize ();
	IDT_initialize ();
	ISR_table_initialize (&null_ISR);

	ISR_table [INT_PIT] = &ISR_PIT;
	IRQ_enable (IRQ_PIT);

	__asm__ ("sti");
	while (true)
		__asm__ ("hlt");

}
