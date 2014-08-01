#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "IRQ.h"
#include "keyboard.h"
#include "format.h"


#include "portio.h"

enum {
	// Port bases
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8,

	// Register indices
	COM_DATA              = 0,
	COM_INTERRUPT_ENABLE  = 1,
	COM_DLAB_DIVISOR_LOW  = 0,
	COM_DLAB_DIVISOR_HIGH = 1,
	COM_INTERRUPT_ID      = 2,
	COM_FIFO_CONTROL      = 2,
	COM_LINE_CONTROL      = 3,
	COM_MODEM_CONTROL     = 4,
	COM_LINE_STATUS       = 5,
	COM_MODEM_STATUS      = 6,
	COM_SCRATCH           = 7,

	// Line control
	COM_CHAR_5_BIT   = 0b00000000,
	COM_CHAR_6_BIT   = 0b00000001,
	COM_CHAR_7_BIT   = 0b00000010,
	COM_CHAR_8_BIT   = 0b00000011,
	COM_STOP_1_BIT   = 0b00000000,
	COM_STOP_2_BIT   = 0b00000100,
	COM_PARITY_NONE  = 0b00000000,
	COM_PARITY_ODD   = 0b00001000,
	COM_PARITY_EVEN  = 0b00011000,
	COM_PARITY_MARK  = 0b00101000,
	COM_PARITY_SPACE = 0b00111000
};

typedef struct {
	uint8_t data_available    : 1;
	uint8_t transmitter_empty : 1;
	uint8_t break_error       : 1;
	uint8_t status_change     : 1;
	uint8_t unused            : 4;
} COM_interrupts;

typedef struct {
	uint8_t char_length : 2;
	uint8_t stop_length : 1;
	uint8_t parity      : 3;
	uint8_t unused      : 1;
	uint8_t enable_DLAB : 1;
} COM_line_control;

void serial_set_interrupts (uint16_t port, COM_interrupts interrupts)
{
	union {
		COM_interrupts interrupts;
		uint8_t value;
	} u = {.interrupts = interrupts};
	outb (port + COM_INTERRUPT_ENABLE, u.value);
}

void serial_set_line_control (uint16_t port, COM_line_control line_control)
{
	union {
		COM_line_control line_control;
		uint8_t value;
	} u = {.line_control = line_control};
	outb (port + COM_LINE_CONTROL, u.value);
}

void serial_setrate (uint16_t port, uint16_t divisor)
{
	serial_set_line_control (port, (COM_line_control) {
		.enable_DLAB = 1
	});
	outb (port + COM_DLAB_DIVISOR_LOW, divisor & 0xFF);
	outb (port + COM_DLAB_DIVISOR_HIGH, (divisor >> 8) & 0xFF);
}

void serial_initialize (uint16_t port, uint16_t divisor)
{
	serial_set_interrupts (port, (COM_interrupts) {
		.data_available    = 0,
		.transmitter_empty = 0,
		.break_error       = 0,
		.status_change     = 0
	});
	serial_setrate (port, divisor);
	serial_set_line_control (port, (COM_line_control) {
		.char_length = COM_CHAR_8_BIT,
		.stop_length = COM_STOP_1_BIT,
		.parity      = COM_PARITY_NONE,
		.enable_DLAB = 0
	});
	serial_set_interrupts (port, (COM_interrupts) {
		.data_available    = 1,
		.transmitter_empty = 0,
		.break_error       = 0,
		.status_change     = 0
	});
	outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
	IRQ_enable (IRQ_COM1);
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

bool basic_keyconsumer (cbuffer* kbuffer)
{
	if (cbuffer_empty (kbuffer))
		return false;

	char buffer [3];
	vga_puts ("Scancode 0x");
	vga_putline (format_uint (buffer, cbuffer_read (kbuffer), 2, 16));
	return true;
}

#include "cbuffer.h"
static cbuffer COM1_buffer;

bool basic_COM1_consumer (void)
{
	bool did_work = false;
	while (!cbuffer_empty (&COM1_buffer)) {
		vga_putchar (cbuffer_read (&COM1_buffer));
		did_work = true;
	}
	return did_work;
}

void serial_ISR (__attribute__ ((unused)) INT_index interrupt)
{
	while (inb (COM1 + COM_LINE_STATUS) & 1) {
		if (cbuffer_full (&COM1_buffer))
			basic_COM1_consumer ();
		cbuffer_write (&COM1_buffer, (inb (COM1 + COM_DATA)));
	}
}

void kernel_main (/* multiboot_info_t* info, uint32_t magic */)
{
	vga_initialize ();
	GDT_initialize ();
	IDT_initialize ();
	ISR_table_initialize (&debug_ISR);
	keyboard_initialize (&basic_keyconsumer);

	COM1_buffer = make_cbuffer ();
	serial_initialize (COM1, 1);
	ISR_table [INT_COM1] = &serial_ISR;

	IRQ_disable (IRQ_PIT);

	while (true) {
		__asm__ ("cli");
		if (!(keyboard_consume () ||
		      basic_COM1_consumer ()))
			__asm__ ("sti\nhlt");
	}

	vga_putline ("System halt");
}
