#include "vga.h"

#define VGA_BASE 0xB8000
#define vga_buffer (*(volatile uint16_t (*) [VGA_HEIGHT] [VGA_WIDTH]) VGA_BASE)

size_t current_row;
size_t current_column;
vga_color current_color;

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
	current_column = 0;
	if (++current_row == VGA_HEIGHT)
		current_row = 0;
}

static inline
void vga_advance_char (void)
{
	if (++current_column == VGA_WIDTH)
	{
		current_column = 0;
		vga_advance_line ();
	}
}


// Extern functions

void vga_initialize (void)
{
	current_row = 0;
	current_column = 0;
	current_color = make_vga_color (COLOR_LIGHT_GREY, COLOR_BLACK);
	for (size_t row = 0; row < VGA_HEIGHT; ++row)
		for (size_t col = 0; col < VGA_WIDTH; ++col)
			vga_buffer [row] [col] = make_vga_entry (' ', current_color).value;
}

void vga_setcolor (vga_color color)
{
	current_color = color;
}

void vga_putchar (char c)
{
	switch (c)
	{
	case '\n':
		vga_advance_line ();
		break;
	default:
		vga_putraw (c, current_color, current_column, current_row);
		vga_advance_char ();
		break;
	}
}

void vga_puts (const char* data)
{
	size_t length = strlen (data);
	for (size_t i = 0; i < length; ++i)
		vga_putchar (data [i]);
	vga_putchar ('\n');
}
