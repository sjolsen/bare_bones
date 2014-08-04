#ifndef CBUFFER_H
#define CBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	uint8_t* buffer;
	size_t size;
	size_t wcursor;
	size_t rcursor;
} cbuffer;

cbuffer make_cbuffer (uint8_t* buffer, size_t size);

bool cbuffer_empty (const cbuffer* buffer);
bool cbuffer_full (const cbuffer* buffer);
void cbuffer_write (cbuffer* buffer, uint8_t code);
uint8_t cbuffer_read (cbuffer* buffer);

#endif
