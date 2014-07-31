#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "IRQ.h"
#include "keyboard.h"
#include "format.h"

bool basic_keyconsumer (keybuffer* kbuffer)
{
	if (keybuffer_empty (kbuffer))
		return false;

	char buffer [3];
	vga_puts ("Scancode 0x");
	vga_putline (format_uint (buffer, keybuffer_read (kbuffer), 2, 16));
	return true;
}

void kernel_main (/* multiboot_info_t* info, uint32_t magic */)
{
	vga_initialize ();
	vga_putline ("VGA initialized");

	GDT_initialize ();
	vga_putline ("GDT initialized");

	IDT_initialize ();
	vga_putline ("IDT initialized");

	ISR_table_initialize (NULL);
	vga_putline ("ISRs initialized");

	keyboard_initialize (&basic_keyconsumer);
	vga_putline ("Keyboard initialized");

	IRQ_disable (IRQ_PIT);

	__asm__ ("sti"); // Enable interrupts
	while (true) {
		if (!(keyboard_consume ()))
			__asm__ ("hlt");
	}

	vga_putline ("System halt");
}
