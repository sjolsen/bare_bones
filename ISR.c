#include "ISR.h"
#include "IRQ.h"
#include "vga.h"
#include "format.h"
#include <stddef.h>


// Extern functions

void ISR_entry (uint32_t interrupt)
{
	if (interrupt == INT_spurious && !IRQ_in_service (INT_LPT1))
		return;

	(*ISR_table [interrupt]) (interrupt);

	if (INT_IRQ_MBASE <= interrupt && interrupt < INT_IRQ_SBASE)
		IRQ_EOI_master ();
	else if (INT_IRQ_SBASE <= interrupt)
		IRQ_EOI_slave ();
}

ISR_t ISR_table [INT_LIMIT];

void null_ISR (__attribute__ ((unused)) INT_index interrupt)
{
}

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
		[0x23] = "IRQ_COM2",
		[0x24] = "IRQ_COM1",
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

void ISR_table_initialize (ISR_t default_ISR)
{
	for (size_t i = 0; i < INT_LIMIT; ++i)
		ISR_table [i] = default_ISR;
}
