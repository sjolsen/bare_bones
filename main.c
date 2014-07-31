#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "IRQ.h"
#include "keyboard.h"
#include "format.h"

void debug_ISR (INT_index interrupt)
{
	static const char* name [] = {
		[0x00] = "divide_by_zero",
		[0x01] = "debugger",
		[0x02] = "NMI",
		[0x03] = "breakpoint",
		[0x04] = "overflow",
		[0x05] = "bounds",
		[0x06] = "invalid_opcode",
		[0x07] = "coprocessor_unavailable",
		[0x08] = "double_fault",
		[0x0A] = "invalid_TSS",
		[0x0B] = "segment_missing",
		[0x0C] = "stack_fault",
		[0x0D] = "protection_fault",
		[0x0E] = "page_fault",
		[0x10] = "math_fault",
		[0x11] = "alignment_check",
		[0x12] = "machine_check",
		[0x13] = "SIMD_exception",

		[0x20] = "IRQ_PIT",
		[0x21] = "IRQ_keyboard",
		[0x22] = "IRQ_cascade",
		[0x23] = "IRQ_COM1",
		[0x24] = "IRQ_COM2",
		[0x25] = "IRQ_LPT2",
		[0x26] = "IRQ_floppy",
		[0x27] = "IRQ_LPT1",
		[0x28] = "IRQ_CMOS_RTC",
		[0x29] = "IRQ_misc1",
		[0x2A] = "IRQ_misc2",
		[0x2B] = "IRQ_misc3",
		[0x2C] = "IRQ_mouse",
		[0x2D] = "IRQ_FPU",
		[0x2E] = "IRQ_HDD1",
		[0x2F] = "IRQ_HDD2",
	};

	char buffer [3];
	vga_puts ("Interrupt 0x");
	vga_puts (format_uint (buffer, interrupt, 2, 16));
	vga_puts (": ");
	vga_putline (name [interrupt]);
}

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
	GDT_initialize ();
	IDT_initialize ();
	ISR_table_initialize (&debug_ISR);
	keyboard_initialize (&basic_keyconsumer);

	IRQ_disable (IRQ_PIT);

	__asm__ ("sti"); // Enable interrupts
	while (true) {
		if (!(keyboard_consume ()))
			__asm__ ("hlt");
	}

	vga_putline ("System halt");
}
