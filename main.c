#include "multiboot.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "8259.h"
#include "portio.h"
#include "keybuffer.h"
#include "vga.h"
#include "format.h"
#include "ddump.h"

static keybuffer kbuffer;

void ISR_alert (uint32_t interrupt)
{
	char buffer [3];
	vga_puts ("Received interrupt 0x");
	vga_putline (format_uint (buffer, interrupt, 2, 16));
}

void ISR_keyboard (uint32_t __attribute__ ((unused)) interrupt)
{
	uint8_t code = inb (0x60);
	keybuffer_write (&kbuffer, code);
}

void kernel_main (/* multiboot_info_t* info, uint32_t magic */)
{
	vga_initialize ();

	GDT_entry GDT [] = {
		make_data_GDT (0, 0, false, false, 0, 0),
		make_code_GDT (0, -1, true, false, 0, 1),
		make_data_GDT (0, -1, true, false, 0, 1)
	};
	install_GDT (GDT, 3);
	reload_segments (1, 2);
	vga_putline ("GDT loaded");

	initialize_IDT (1);
	initialize_ISR_table ();
	vga_putline ("IDT loaded");

	for (int i = 0; i < 0x30; ++i)
		ISR_table [i] = &ISR_alert;
	ISR_table [0x21] = &ISR_keyboard;
	vga_putline ("Installed ISRs");

	IRQ_disable (0);
	IRQ_enable (1);

	kbuffer = make_keybuffer ();

	__asm__ ("sti"); // Enable interrupts

	while (true) {
		if (keybuffer_empty (&kbuffer))
			__asm__ ("hlt");
		else {
			char buffer [3];
			vga_puts ("Scancode 0x");
			vga_putline (format_uint (buffer, keybuffer_read (&kbuffer), 2, 16));
		}
	}

	vga_putline ("System halt");
}
