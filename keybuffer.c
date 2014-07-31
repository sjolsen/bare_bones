#include "keybuffer.h"

static inline
size_t next (size_t cursor)
{
	return (cursor + 1) % KEYBUFFER_SIZE;
}


// Extern functions

keybuffer make_keybuffer (void)
{
	return (keybuffer) {
		.wcursor = 0,
		.rcursor = 0
	};
}

bool keybuffer_empty (const keybuffer* buffer)
{
	return buffer->rcursor == buffer->wcursor;
}

bool keybuffer_full (const keybuffer* buffer)
{
	return next (buffer->wcursor) == buffer->rcursor;
}

void keybuffer_write (keybuffer* buffer, uint8_t code)
{
	buffer->buffer [buffer->wcursor] = code;
	buffer->wcursor = next (buffer->wcursor);
}

uint8_t keybuffer_read (keybuffer* buffer)
{
	uint8_t code = buffer->buffer [buffer->rcursor];
	buffer->rcursor = next (buffer->rcursor);
	return code;
}
