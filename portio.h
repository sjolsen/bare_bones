#ifndef PORTIO_H
#define PORTIO_H

// Should be 16-bit
typedef enum {
	PIC1_COMMAND = 0x20,
	PIC1_DATA    = 0x21,
	PIC2_COMMAND = 0xA0,
	PIC2_DATA    = 0xA1,
} io_port;

static inline
void outb (io_port port, uint8_t value)
{
	uint16_t port16 = (uint16_t) port;
	__asm__ ("outb %0, %1" :: "a" (value), "d" (port16));
}

static inline
uint8_t inb (io_port port)
{
	uint16_t port16 = (uint16_t) port;
	uint8_t value;
	__asm__ ("inb %1, %0" : "=a" (value) : "d" (port16));
	return value;
}

static inline
void outw (io_port port, uint16_t value)
{
	uint16_t port16 = (uint16_t) port;
	__asm__ ("outw %0, %1" :: "a" (value), "d" (port16));
}

static inline
uint16_t inw (io_port port)
{
	uint16_t port16 = (uint16_t) port;
	uint16_t value;
	__asm__ ("inw %1, %0" : "=a" (value) : "d" (port16));
	return value;
}

#endif
