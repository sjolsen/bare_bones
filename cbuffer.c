#include "cbuffer.h"

static inline
size_t next (size_t cursor)
{
	return (cursor + 1) % CBUFFER_SIZE;
}


// Extern functions

cbuffer make_cbuffer (void)
{
	return (cbuffer) {
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
	return next (buffer->wcursor) == buffer->rcursor;
}

void cbuffer_write (cbuffer* buffer, uint8_t code)
{
	buffer->buffer [buffer->wcursor] = code;
	buffer->wcursor = next (buffer->wcursor);
}

uint8_t cbuffer_read (cbuffer* buffer)
{
	uint8_t code = buffer->buffer [buffer->rcursor];
	buffer->rcursor = next (buffer->rcursor);
	return code;
}
