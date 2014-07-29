#include "8259.h"

enum {
	// 8259A PIC I/O Ports
	PIC1_COMMAND = 0x20,
	PIC1_DATA    = 0x21,
	PIC2_COMMAND = 0xA0,
	PIC2_DATA    = 0xA1,

	// 8259A Initialization Control Word 1
	ICW1           = 0x10, // Base for ICW1
	ICW1_NEED_ICW4 = 0x01, // Require ICW4
	ICW1_SINGLE    = 0x02, // Skip ICW3 (single PIC)
	ICW1_INTERVAL4 = 0x04, // 4-byte IVT packing (no effect on 8086)
	ICW1_LEVELTRIG = 0x08, // Level-triggered (default edge-triggered)

	// ICW2 specifies the 5 most-significant bits of the PIC's IVT. The low
	// three bits are ignored.

	// ICW3 (master PIC) specifies the IRQs to be delegated to slave
	// PICs. 8086 convention expects the slave PIC on IRQ2, so we set bit
	// two high.
	ICW3_MASTER = 0x04,
	// ICW3 (slave PIC) specifies the IRQ to be delegated to it. IRQ2 is
	// 0b010.
	ICW3_SLAVE  = 0x02,

	// 8259A Initialization Control Word 4
	ICW4_SFNM     = 0x10, // Special fully nested mode
	ICW4_BUFFERED = 0x08, // Buffered mode
	ICW4_MASTER   = 0x04, // Master (default slave, no effect outside of buffered mode)
	ICW4_AUTO_EOI = 0x02, // Automatic end-of-interrupt
	ICW4_8086     = 0x01  // 8086 mode (default MCS-80/85)
};


// Extern functions

void remap_8259_PIC (uint8_t master_base, uint8_t slave_base)
{
	uint8_t master_mask = inb (PIC1_DATA);
	uint8_t slave_mask  = inb (PIC2_DATA);

	outb (PIC1_COMMAND, ICW1 | ICW1_NEED_ICW4);
	outb (PIC1_DATA, master_base);
	outb (PIC1_DATA, ICW3_MASTER);
	outb (PIC1_DATA, ICW4_8086 | ICW4_BUFFERED | ICW4_MASTER);
}
