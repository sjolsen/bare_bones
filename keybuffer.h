#ifndef KEYBUFFER_H
#define KEYBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum {
	KEYBUFFER_SIZE = 32
};

typedef struct {
	uint8_t buffer [KEYBUFFER_SIZE];
	size_t wcursor;
	size_t rcursor;
} keybuffer;

keybuffer make_keybuffer (void);

bool keybuffer_empty (const keybuffer* buffer);
bool keybuffer_full (const keybuffer* buffer);
void keybuffer_write (keybuffer* buffer, uint8_t code);
uint8_t keybuffer_read (keybuffer* buffer);

#endif
