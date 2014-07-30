#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "8259.h"
#include "ddump.h"

void ISR_alert (uint32_t interrupt)
{
	char buffer [3];
	vga_puts ("Received interrupt 0x");
	vga_putline (format_uint (buffer, interrupt, 2, 16));
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
	vga_putline ("Installed alert ISR");

	IRQ_disable (0);
	IRQ_enable (1);

	__asm__ ("sti");
	while (true)
		__asm__ ("hlt");

	vga_putline ("System halt");
}
