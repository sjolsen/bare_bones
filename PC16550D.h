#ifndef PC16550D_H
#define PC16550D_H

#include <stdint.h>

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

void UART_PC16550D_setrate (uint16_t port, uint16_t divisor);
void UART_PC16550D_initialize (uint16_t port, uint16_t divisor,
                               COM_line_control linectl,
                               COM_interrupts interrupts,
                               COM_fifo_control fifoctl);
#endif
