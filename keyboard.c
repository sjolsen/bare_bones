#include "keyboard.h"
#include "portio.h"
#include "ISR.h"
#include "IRQ.h"

enum {
	KBUFFER_SIZE = 32
};

static uint8_t kbuffer_store [KBUFFER_SIZE];
static cbuffer kbuffer;
static keyboard_consumer_t kconsumer;

static
void ISR_keyboard (__attribute__ ((unused)) INT_index interrupt)
{
	// FIXME: Hardcoded constant
	uint8_t code = inb (0x60);
	cbuffer_write (&kbuffer, code);
}


// Extern functions

void keyboard_initialize (keyboard_consumer_t consumer)
{
	kbuffer = make_cbuffer (kbuffer_store, KBUFFER_SIZE);
	kconsumer = consumer;
	ISR_table [INT_keyboard] = &ISR_keyboard;
	IRQ_enable (1);
}

bool keyboard_consume (void)
{
	return (*kconsumer) (&kbuffer);
}
