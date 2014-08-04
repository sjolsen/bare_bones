#include "cbuffer.h"

static inline
size_t next (size_t cursor, size_t size)
{
	return (cursor + 1) % size;
}


// Extern functions

cbuffer make_cbuffer (uint8_t* buffer, size_t size)
{
	return (cbuffer) {
		.buffer  = buffer,
		.size    = size,
		.wcursor = 0,
		.rcursor = 0
	};
}

bool cbuffer_empty (const cbuffer* buffer)
{
	return buffer->rcursor == buffer->wcursor;
}

bool cbuffer_full (const cbuffer* buffer)
{
	return next (buffer->wcursor, buffer->size) == buffer->rcursor;
}

void cbuffer_write (cbuffer* buffer, uint8_t code)
{
	buffer->buffer [buffer->wcursor] = code;
	buffer->wcursor = next (buffer->wcursor, buffer->size);
}

uint8_t cbuffer_read (cbuffer* buffer)
{
	uint8_t code = buffer->buffer [buffer->rcursor];
	buffer->rcursor = next (buffer->rcursor, buffer->size);
	return code;
}
