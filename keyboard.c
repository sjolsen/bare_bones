#include "keyboard.h"
#include "portio.h"
#include "ISR.h"
#include "IRQ.h"

static cbuffer kbuffer;
static keyboard_consumer_t kconsumer;

static
bool null_kconsumer (__attribute__ ((unused)) cbuffer* kbuffer)
{
	return false;
}

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
	kbuffer = make_cbuffer ();
	kconsumer = (consumer ? consumer : &null_kconsumer);
	ISR_table [INT_keyboard] = &ISR_keyboard;
	IRQ_enable (1);
}

bool keyboard_consume (void)
{
	return (*kconsumer) (&kbuffer);
}
