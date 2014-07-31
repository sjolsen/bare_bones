#include "keyboard.h"
#include "portio.h"
#include "ISR.h"
#include "8259.h"

static keybuffer kbuffer;
static keyboard_consumer_t kconsumer;

static
bool null_kconsumer (keybuffer* __attribute__ ((unused)) kbuffer)
{
	return false;
}

static
void ISR_keyboard (uint32_t __attribute__ ((unused)) interrupt)
{
	uint8_t code = inb (0x60);
	keybuffer_write (&kbuffer, code);
}


// Extern functions

void keyboard_initialize (keyboard_consumer_t consumer)
{
	kbuffer = make_keybuffer ();
	kconsumer = (consumer ? consumer : &null_kconsumer);
	ISR_table [0x21] = &ISR_keyboard;
	IRQ_enable (1);
}

bool keyboard_consume (void)
{
	return (*kconsumer) (&kbuffer);
}
