#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "IRQ.h"
#include "keyboard.h"
#include "format.h"
#include "PC16550D.h"
#include "portio.h"
#include "cbuffer.h"
#include "string.h"
#include "scancode.h"

static inline
void sanitarily_print_char (char c)
{
	if (ascii_printable (c) || c == '\n')
		vga_putchar (c);
	else {
		vga_color oldcolor = vga_getcolor ();
		vga_setcolor (make_vga_color (oldcolor.bg, oldcolor.fg));
		vga_putchar (c ^ 0b100000);
		vga_setcolor (oldcolor);
	}
}

static inline
void send_COM1 (uint8_t byte)
{
	outb (COM1 + COM_DATA, byte);
}

static scancode_decoder_state dstate;
static bool lshift = false;
static bool rshift = false;

bool basic_keyconsumer (cbuffer* kbuffer)
{
	if (cbuffer_empty (kbuffer))
		return false;

	#define INRANGE(x,a,b) ((a) <= (x) && (x) <= (b))
	while (!cbuffer_empty (kbuffer)) {
		key_event e = scancode_decode (&dstate, cbuffer_read (kbuffer));

		if (e.type == TYPE_PRESSED) {
			if (e.key == KEY_LEFT_SHIFT)
				lshift = true;
			else if (e.key == KEY_RIGHT_SHIFT)
				rshift = true;
			else if (lshift || rshift) {
				if (INRANGE (e.key, KEY_1, KEY_EQUAL))
					send_COM1 ("!@#$%^&*()_+" [e.key - KEY_1]);
				else if (INRANGE (e.key, KEY_Q, KEY_ENTER))
					send_COM1 ("QWERTYUIOP{}\n" [e.key - KEY_Q]);
				else if (INRANGE (e.key, KEY_A, KEY_BACKTICK))
					send_COM1 ("ASDFGHJKL:\"~" [e.key - KEY_A]);
				else if (INRANGE (e.key, KEY_BACKSLASH, KEY_SLASH))
					send_COM1 ("|ZXCVBNM<>?" [e.key - KEY_BACKSLASH]);
				else if (e.key == KEY_SPACE)
					send_COM1 (' ');
			}
			else {
				if (INRANGE (e.key, KEY_1, KEY_EQUAL))
					send_COM1 ("1234567890-=" [e.key - KEY_1]);
				else if (INRANGE (e.key, KEY_Q, KEY_ENTER))
					send_COM1 ("qwertyuiop[]\n" [e.key - KEY_Q]);
				else if (INRANGE (e.key, KEY_A, KEY_BACKTICK))
					send_COM1 ("asdfghjkl;'`" [e.key - KEY_A]);
				else if (INRANGE (e.key, KEY_BACKSLASH, KEY_SLASH))
					send_COM1 ("\\zxcvbnm,./" [e.key - KEY_BACKSLASH]);
				else if (e.key == KEY_SPACE)
					send_COM1 (' ');
			}
		}
		else if (e.type == TYPE_RELEASED) {
			if (e.key == KEY_LEFT_SHIFT)
				lshift = false;
			else if (e.key == KEY_RIGHT_SHIFT)
				rshift = false;
		}
	}

	return true;
}

enum { COM1_BUFFER_SIZE = 32 };
static uint8_t COM1_buffer_store [COM1_BUFFER_SIZE];
static cbuffer COM1_buffer;

bool basic_COM1_consumer (void)
{
	bool did_work = false;
	while (!cbuffer_empty (&COM1_buffer)) {
		uint8_t data = cbuffer_read (&COM1_buffer);
		sanitarily_print_char (data);
		did_work = true;
	}
	return did_work;
}

void ISR_serial (__attribute__ ((unused)) INT_index interrupt)
{
	while (inb (COM1 + COM_LINE_STATUS) & 1) {
		if (cbuffer_full (&COM1_buffer))
			basic_COM1_consumer ();
		cbuffer_write (&COM1_buffer, (inb (COM1 + COM_DATA)));
	}
}

static uint32_t counter = 0;

void ISR_PIT (__attribute__ ((unused)) INT_index interrupt)
{
	char buffer [11];
	vga_putline (format_uint (buffer, counter, 0, 10));
	++counter;
}

void kernel_main (/* multiboot_info_t* info, uint32_t magic */)
{
	vga_initialize ();
	GDT_initialize ();
	IDT_initialize ();
	ISR_table_initialize (&debug_ISR);
	keyboard_initialize (&basic_keyconsumer);
	dstate = make_decoder_state ();

	COM_line_control line_8N1 = {{
		.char_length = COM_CHAR_8_BIT,
		.stop_length = COM_STOP_1_BIT,
		.parity      = COM_PARITY_NONE
	}};
	COM_interrupts interrupts = {{
		.data_available    = 1,
		.transmitter_empty = 0,
		.break_error       = 0,
		.status_change     = 0
	}};
	COM_fifo_control fifoctl = {{
		.enable_fifo       = 1,
		.reset_receiver    = 1,
		.reset_transmitter = 1,
		.DMA_mode_select   = 0,
		.receiver_trigger  = COM_FIFO_TRIGGER_8_BYTE
	}};
	UART_PC16550D_initialize (COM1, 1, line_8N1, interrupts, fifoctl);
	ISR_table [INT_COM1] = &ISR_serial;
	IRQ_enable (IRQ_COM1);
	COM1_buffer = make_cbuffer (COM1_buffer_store, COM1_BUFFER_SIZE);

	IRQ_disable (IRQ_PIT);

	while (true) {
		__asm__ ("cli");
		if (!(keyboard_consume () ||
		      basic_COM1_consumer ()))
			__asm__ ("sti\nhlt");
	}
}
