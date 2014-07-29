#include "vga.h"
#include "format.h"

static inline
uint32_t cr0 (void)
{
	uint32_t ret;
	__asm__ ("movl %%cr0, %0" : "=r" (ret));
	return ret;
}

void kernel_main (void)
{
	vga_initialize ();

	char buffer [12];

	vga_puts ("cr0: ");
	vga_putline (format_uint32_t (buffer, cr0 ()));

	vga_puts ("INT_MIN: ");
	vga_putline (format_int32_t (buffer, (int) 0x80000000));

	vga_putline ("System halt");
}
