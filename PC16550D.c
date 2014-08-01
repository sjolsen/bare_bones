#include "PC16550D.h"
#include "portio.h"


// Extern functions

void UART_PC16550D_setrate (uint16_t port, uint16_t divisor)
{
	outb (port + COM_LINE_CONTROL, ((COM_line_control) {{
		.enable_DLAB = 1
	}}).value);
	outb (port + COM_DLAB_DIVISOR_LOW, divisor & 0xFF);
	outb (port + COM_DLAB_DIVISOR_HIGH, (divisor >> 8) & 0xFF);
}

void UART_PC16550D_initialize (uint16_t port, uint16_t divisor,
                               COM_line_control linectl,
                               COM_interrupts interrupts,
                               COM_fifo_control fifoctl)
{
	// Disable interrupts
	outb (port + COM_INTERRUPT_ENABLE, ((COM_interrupts) {{
		.data_available    = 0,
		.transmitter_empty = 0,
		.break_error       = 0,
		.status_change     = 0
	}}).value);
	// Set the baud rate
	UART_PC16550D_setrate (port, divisor);
	// Set the line control
	linectl.enable_DLAB = 0;
	outb (port + COM_LINE_CONTROL, linectl.value);
	// Enable and clear FIFO
	outb (port + COM_FIFO_CONTROL, fifoctl.value);
	// Reenable interrupts
	outb (port + COM_INTERRUPT_ENABLE, interrupts.value);
}
