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
	COM_PARITY_SPACE = 0b00111000,

	// FIFO control
	COM_FIFO_TRIGGER_1_BYTE  = 0,
	COM_FIFO_TRIGGER_4_BYTE  = 1,
	COM_FIFO_TRIGGER_8_BYTE  = 2,
	COM_FIFO_TRIGGER_14_BYTE = 3,

	// Interrupts
	COM_INT_LINE_STATUS       = 0b0110, // Read Line Status register
	COM_INT_DATA_AVAILABLE    = 0b0100, // Read Receiver Buffer register
	COM_INT_RECEIVER_TIMEOUT  = 0b1100, // Read Receiver Buffer register
	COM_INT_TRANSMITTER_EMPTY = 0b0010, // Read Interrupt ID register
	COM_INT_MODEM_STATUS      = 0b0000, // Read Modem Status register
};

typedef union {
	struct {
		uint8_t data_available    : 1;
		uint8_t transmitter_empty : 1;
		uint8_t break_error       : 1;
		uint8_t status_change     : 1;
		uint8_t unused            : 4;
	};
	uint8_t value;
} COM_interrupts;

typedef union {
	struct {
		uint8_t char_length : 2;
		uint8_t stop_length : 1;
		uint8_t parity      : 3;
		uint8_t unused      : 1;
		uint8_t enable_DLAB : 1;
	};
	uint8_t value;
} COM_line_control;

typedef union {
	struct {
		uint8_t enable_fifo       : 1;
		uint8_t reset_receiver    : 1;
		uint8_t reset_transmitter : 1;
		uint8_t DMA_mode_select   : 1;
		uint8_t unused            : 2;
		uint8_t receiver_trigger  : 2;
	};
	uint8_t value;
} COM_fifo_control;

void serial_setrate (uint16_t port, uint16_t divisor)
{
	outb (port + COM_LINE_CONTROL, ((COM_line_control) {{
		.enable_DLAB = 1
	}}).value);
	outb (port + COM_DLAB_DIVISOR_LOW, divisor & 0xFF);
	outb (port + COM_DLAB_DIVISOR_HIGH, (divisor >> 8) & 0xFF);
}

void serial_initialize (uint16_t port, uint16_t divisor)
{
	// Disable interrupts
	outb (port + COM_INTERRUPT_ENABLE, ((COM_interrupts) {{
		.data_available    = 0,
		.transmitter_empty = 0,
		.break_error       = 0,
		.status_change     = 0
	}}).value);
	// Set the baud rate
	serial_setrate (port, divisor);
	// Set the line discipline
	outb (port + COM_LINE_CONTROL, ((COM_line_control) {{
		.char_length = COM_CHAR_8_BIT,
		.stop_length = COM_STOP_1_BIT,
		.parity      = COM_PARITY_NONE,
		.enable_DLAB = 0
	}}).value);
	// Enable and clear FIFO
	outb (port + COM_FIFO_CONTROL, ((COM_fifo_control) {{
		.enable_fifo       = 1,
		.reset_receiver    = 1,
		.reset_transmitter = 1,
		.DMA_mode_select   = 0,
		.receiver_trigger  = COM_FIFO_TRIGGER_14_BYTE
	}}).value);
	// Reenable interrupts
	outb (port + COM_INTERRUPT_ENABLE, ((COM_interrupts) {{
		.data_available    = 1,
		.transmitter_empty = 0,
		.break_error       = 0,
		.status_change     = 0
	}}).value);
	IRQ_enable (IRQ_COM1);
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
