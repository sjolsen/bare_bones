#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "8259.h"
#include "portio.h"
#include "ddump.h"

uint8_t keybuffer [256];
size_t keybuffer_wcur = 0;
size_t keybuffer_rcur = 0;

static inline
size_t keybuffer_next (size_t cur)
{
	return (cur + 1) % 256;
}

static inline
bool keybuffer_empty (void)
{
	return keybuffer_rcur == keybuffer_wcur;
}

static inline
bool keybuffer_full (void)
{
	return keybuffer_next (keybuffer_wcur) == keybuffer_rcur;
}

static inline
void keybuffer_write (uint8_t code)
{
	if (keybuffer_full ())
		vga_putline ("Keyboard buffer overflow");
	else {
		keybuffer [keybuffer_wcur] = code;
		keybuffer_wcur = keybuffer_next (keybuffer_wcur);
	}
}

static inline
uint8_t keybuffer_read (void)
{
	if (keybuffer_empty ()) {
		vga_putline ("Keyboard buffer underflow");
		return -1;
	}
	else {
		uint8_t code = keybuffer [keybuffer_rcur];
		keybuffer_rcur = keybuffer_next (keybuffer_rcur);
		return code;
	}
}



void ISR_alert (uint32_t interrupt)
{
	char buffer [3];
	vga_puts ("Received interrupt 0x");
	vga_putline (format_uint (buffer, interrupt, 2, 16));
}

void ISR_keyboard (uint32_t interrupt)
{
	uint8_t code = inb (0x60);
	if (!(code & 0x80)) // Pressed
		keybuffer_write (code);
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

	__asm__ ("sti"); // Enable interrupts

	while (true) {
		if (keybuffer_empty ())
			__asm__ ("hlt");
		else {
			char buffer [3];
			vga_puts ("Scancode 0x");
			vga_putline (format_uint (buffer, keybuffer_read (), 2, 16));
		}
	}

	vga_putline ("System halt");
}
