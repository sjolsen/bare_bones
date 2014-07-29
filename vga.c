#include "vga.h"

#define VGA_BASE 0xB8000
#define vga_buffer (*(volatile uint16_t (*) [VGA_HEIGHT] [VGA_WIDTH]) VGA_BASE)

size_t vga_current_row;
size_t vga_current_column;
vga_color vga_current_color;

static inline
size_t strlen (const char* str)
{
	size_t length = 0;
	while (str [length] != '\0')
		++length;
	return length;
}

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


// Extern functions

void vga_initialize (void)
{
	vga_current_row = 0;
	vga_current_column = 0;
	vga_current_color = make_vga_color (COLOR_LIGHT_GREY, COLOR_BLACK);
	for (size_t row = 0; row < VGA_HEIGHT; ++row)
		for (size_t col = 0; col < VGA_WIDTH; ++col)
			vga_buffer [row] [col] = make_vga_entry (' ', vga_current_color).value;
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
}

void vga_putchar (char c)
{
	switch (c)
	{
	case '\n':
		vga_advance_line ();
		break;
	default:
		vga_putraw (c, vga_current_color, vga_current_column, vga_current_row);
		vga_advance_char ();
		break;
	}
}

void vga_puts (const char* data)
{
	size_t length = strlen (data);
	for (size_t i = 0; i < length; ++i)
		vga_putchar (data [i]);
}

void vga_putline (const char* data)
{
	vga_puts (data);
	vga_putchar ('\n');
}
