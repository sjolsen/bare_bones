#ifndef PORTIO_H
#define PORTIO_H

static inline
void outb (uint16_t port, uint8_t value)
{
	__asm__ ("outb %0, %1" :: "a" (value), "d" (port));
}

static inline
uint8_t inb (uint16_t port)
{
	uint8_t value;
	__asm__ ("inb %1, %0" : "=a" (value) : "d" (port));
	return value;
}

static inline
void outw (uint16_t port, uint16_t value)
{
	__asm__ ("outw %0, %1" :: "a" (value), "d" (port));
}

static inline
uint16_t inw (uint16_t port)
{
	uint16_t value;
	__asm__ ("inw %1, %0" : "=a" (value) : "d" (port));
	return value;
}

#endif
