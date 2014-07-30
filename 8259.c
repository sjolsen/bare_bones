#include "8259.h"
#include "portio.h"

enum {
	// 8259A PIC I/O Ports
	PIC1_COMMAND = 0x20,
	PIC1_DATA    = 0x21,
	PIC2_COMMAND = 0xA0,
	PIC2_DATA    = 0xA1,

	// End-of-interrupt command
	EOI = 0x20,

	// 8259A Initialization Control Word 1
	ICW1           = 0x10, // Base for ICW1
	ICW1_NEED_ICW4 = 0x01, // Require ICW4
	ICW1_SINGLE    = 0x02, // Skip ICW3 (single PIC)
	ICW1_INTERVAL4 = 0x04, // 4-byte IVT packing (no effect on 8086)
	ICW1_LEVELTRIG = 0x08, // Level-triggered (default edge-triggered)

	// ICW2 specifies the 5 most-significant bits of the PIC's IVT. The low
	// three bits are ignored.

	// 8259A Initialization Control Word 3
	ICW3_MASTER = 0x04, // Slave on IRQ2 (bit 2)
	ICW3_SLAVE  = 0x02, // Slave on IRQ2 (binary 010)

	// 8259A Initialization Control Word 4
	ICW4_SFNM     = 0x10, // Special fully nested mode
	ICW4_BUFFERED = 0x08, // Buffered mode
	ICW4_MASTER   = 0x04, // Master (default slave, no effect outside of buffered mode)
	ICW4_AUTO_EOI = 0x02, // Automatic end-of-interrupt
	ICW4_8086     = 0x01, // 8086 mode (default MCS-80/85)

	// 8259A Operation Control Word 3
	OCW3      = 0x08, // Base for OCW3
	OCW3_READ = 0x04, // Read register
	OCW3_IRR  = 0x00, // Read IRR
	OCW3_ISR  = 0x01  // Read ISR
};

static inline
uint8_t IRQ_read_register (uint8_t IRQ, uint8_t OCW3_register)
{
	uint16_t port = (IRQ < 8) ? PIC1_COMMAND : PIC2_COMMAND;
	uint8_t index = IRQ % 8;
	outb (port, OCW3 | OCW3_READ | OCW3_register);
	return inb (port) & (1 << index);
}


// Extern functions

void remap_8259_PIC (uint8_t master_base, uint8_t slave_base)
{
	uint8_t master_mask = inb (PIC1_DATA);
	uint8_t slave_mask  = inb (PIC2_DATA);

	outb (PIC1_COMMAND, ICW1 | ICW1_NEED_ICW4);
	outb (PIC1_DATA, master_base);
	outb (PIC1_DATA, ICW3_MASTER);
	outb (PIC1_DATA, ICW4_8086);

	outb (PIC2_COMMAND, ICW1 | ICW1_NEED_ICW4);
	outb (PIC2_DATA, slave_base);
	outb (PIC2_DATA, ICW3_SLAVE);
	outb (PIC2_DATA, ICW4_8086);

	outb (PIC1_DATA, master_mask);
	outb (PIC1_DATA, slave_mask);
}

void IRQ_EOI_master (void)
{
	outb (PIC1_COMMAND, EOI);
}

void IRQ_EOI_slave (void)
{
	outb (PIC1_COMMAND, EOI);
	outb (PIC2_COMMAND, EOI);
}

void IRQ_disable (uint8_t IRQ)
{
	uint16_t port = (IRQ < 8) ? PIC1_DATA : PIC2_DATA;
	uint8_t index = IRQ % 8;
	outb (port, inb (port) | (1 << index));
}

void IRQ_enable (uint8_t IRQ)
{
	uint16_t port = (IRQ < 8) ? PIC1_DATA : PIC2_DATA;
	uint8_t index = IRQ % 8;
	outb (port, inb (port) & ~(1 << index));
}

bool IRQ_requested (uint8_t IRQ)
{
	return IRQ_read_register (IRQ, OCW3_IRR);
}

bool IRQ_in_service (uint8_t IRQ)
{
	return IRQ_read_register (IRQ, OCW3_ISR);
}
