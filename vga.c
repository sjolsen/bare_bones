#include "vga.h"
#include "string.h"
#include "portio.h"

#define VGA_BASE 0xB8000
#define vga_buffer (*(volatile uint16_t (*) [VGA_HEIGHT] [VGA_WIDTH]) VGA_BASE)

size_t vga_current_row;
size_t vga_current_column;
vga_color vga_current_color;
uint16_t vga_io_base;

static inline
void vga_putraw (char c, vga_color color, size_t col, size_t row)
{
	vga_buffer [row] [col] = make_vga_entry (c, color).value;
}

static inline
void vga_advance_line (void)
{
	vga_current_column = 0;
	if (++vga_current_row >= VGA_HEIGHT)
		vga_scroll (1);
}

static inline
void vga_advance_char (void)
{
	if (++vga_current_column >= VGA_WIDTH)
	{
		vga_current_column = 0;
		vga_advance_line ();
	}
}

static inline
void vga_draw_cursor (void)
{
	uint16_t position = VGA_WIDTH * vga_current_row + vga_current_column;
	outb (vga_io_base, 0x0F);
	outb (vga_io_base + 1, position & 0xFF);
	outb (vga_io_base, 0x0E);
	outb (vga_io_base + 1, position >> 8);
}

static inline
void vga_putchar_impl (char c)
{
	if (c == '\n')
		vga_advance_line ();
	else if (!ascii_printable (c)) {
		vga_color oldcolor = vga_getcolor ();
		vga_setcolor (make_vga_color (COLOR_RED, oldcolor.bg));
		vga_putchar ('?');
		vga_setcolor (oldcolor);
	}
	else {
		vga_putraw (c, vga_current_color, vga_current_column, vga_current_row);
		vga_advance_char ();
	}

	vga_draw_cursor ();
}


// Extern functions

void vga_initialize (void)
{
	vga_current_row = 0;
	vga_current_column = 0;
	vga_current_color = make_vga_color (COLOR_LIGHT_GREY, COLOR_BLACK);
	vga_io_base = *((volatile uint16_t*) 0x0463);
	for (size_t row = 0; row < VGA_HEIGHT; ++row)
		for (size_t col = 0; col < VGA_WIDTH; ++col)
			vga_buffer [row] [col] = make_vga_entry (' ', vga_current_color).value;
	vga_draw_cursor ();
}

vga_color vga_getcolor (void)
{
	return vga_current_color;
}

void vga_setcolor (vga_color color)
{
	vga_current_color = color;
}

void vga_scroll (size_t lines)
{
	if (lines == 0)
		return;
	if (vga_current_row < lines)
		lines = vga_current_row;
	vga_current_row -= lines;

	volatile uint16_t* dst = vga_buffer [0];
	volatile uint16_t* src = vga_buffer [lines];
	volatile uint16_t* const dst_end = dst + VGA_WIDTH * (VGA_HEIGHT - lines);
	volatile uint16_t* const src_end = src + VGA_WIDTH * (VGA_HEIGHT - lines);

	while (dst != dst_end)
		*dst++ = *src++;
	while (dst != src_end)
		*dst++ = make_vga_entry (' ', vga_current_color).value;

	vga_draw_cursor ();
}

void vga_putchar (char c)
{
	vga_putchar_impl (c);
	vga_draw_cursor ();
}

void vga_puts (const char* data)
{
	size_t length = strlen (data);
	for (size_t i = 0; i < length; ++i)
		vga_putchar_impl (data [i]);
	vga_draw_cursor ();
}

void vga_putline (const char* data)
{
	vga_puts (data);
	vga_putchar ('\n');
}
