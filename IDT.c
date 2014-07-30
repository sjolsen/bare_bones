#include "IDT.h"
#include "8259.h"
#include "isr_stub.h"

static
IDT_entry IDT [0x30];

static
IDT_entry make_IDT_entry (void (*address) (void), uint16_t code_descriptor)
{
	enum {
		TASK_32 = 0x5,
		INTR_16 = 0x6,
		TRAP_16 = 0x7,
		INTR_32 = 0xE,
		TRAP_32 = 0xF
	};

	return (IDT_entry) {
		.address_low  = (uintptr_t) address & 0xFFFF,
		.selector     = code_descriptor * sizeof (IDT_entry),
		.zero1        = 0,
		.gate_type    = INTR_32,
		.zero2        = 0,
		.privilege    = 0,
		.present      = 1,
		.address_high = ((uintptr_t) address >> 16) & 0xFFFF
	};
}

static
void install_IDT (const IDT_entry* base, uint16_t entries)
{
	struct __attribute__ ((packed)) {
		uint16_t length;
		uint32_t base;
	} IDT = {entries * sizeof (IDT_entry), (uintptr_t) base};
	__asm__ ("lidt (%0)" :: "p" (&IDT));
}


// Extern functions

void initialize_IDT (uint16_t code_descriptor)
{
	remap_8259_PIC (0x20, 0x28);

	IDT [0x00] = make_IDT_entry (&_ISR_00, code_descriptor);
	IDT [0x01] = make_IDT_entry (&_ISR_01, code_descriptor);
	IDT [0x02] = make_IDT_entry (&_ISR_02, code_descriptor);
	IDT [0x03] = make_IDT_entry (&_ISR_03, code_descriptor);
	IDT [0x04] = make_IDT_entry (&_ISR_04, code_descriptor);
	IDT [0x05] = make_IDT_entry (&_ISR_05, code_descriptor);
	IDT [0x06] = make_IDT_entry (&_ISR_06, code_descriptor);
	IDT [0x07] = make_IDT_entry (&_ISR_07, code_descriptor);
	IDT [0x08] = make_IDT_entry (&_ISR_08, code_descriptor);
	IDT [0x09] = make_IDT_entry (&_ISR_09, code_descriptor);
	IDT [0x0A] = make_IDT_entry (&_ISR_0A, code_descriptor);
	IDT [0x0B] = make_IDT_entry (&_ISR_0B, code_descriptor);
	IDT [0x0C] = make_IDT_entry (&_ISR_0C, code_descriptor);
	IDT [0x0D] = make_IDT_entry (&_ISR_0D, code_descriptor);
	IDT [0x0E] = make_IDT_entry (&_ISR_0E, code_descriptor);
	IDT [0x0F] = make_IDT_entry (&_ISR_0F, code_descriptor);
	IDT [0x10] = make_IDT_entry (&_ISR_10, code_descriptor);
	IDT [0x11] = make_IDT_entry (&_ISR_11, code_descriptor);
	IDT [0x12] = make_IDT_entry (&_ISR_12, code_descriptor);
	IDT [0x13] = make_IDT_entry (&_ISR_13, code_descriptor);
	IDT [0x14] = make_IDT_entry (&_ISR_14, code_descriptor);
	IDT [0x15] = make_IDT_entry (&_ISR_15, code_descriptor);
	IDT [0x16] = make_IDT_entry (&_ISR_16, code_descriptor);
	IDT [0x17] = make_IDT_entry (&_ISR_17, code_descriptor);
	IDT [0x18] = make_IDT_entry (&_ISR_18, code_descriptor);
	IDT [0x19] = make_IDT_entry (&_ISR_19, code_descriptor);
	IDT [0x1A] = make_IDT_entry (&_ISR_1A, code_descriptor);
	IDT [0x1B] = make_IDT_entry (&_ISR_1B, code_descriptor);
	IDT [0x1C] = make_IDT_entry (&_ISR_1C, code_descriptor);
	IDT [0x1D] = make_IDT_entry (&_ISR_1D, code_descriptor);
	IDT [0x1E] = make_IDT_entry (&_ISR_1E, code_descriptor);
	IDT [0x1F] = make_IDT_entry (&_ISR_1F, code_descriptor);
	IDT [0x20] = make_IDT_entry (&_ISR_20, code_descriptor);
	IDT [0x21] = make_IDT_entry (&_ISR_21, code_descriptor);
	IDT [0x22] = make_IDT_entry (&_ISR_22, code_descriptor);
	IDT [0x23] = make_IDT_entry (&_ISR_23, code_descriptor);
	IDT [0x24] = make_IDT_entry (&_ISR_24, code_descriptor);
	IDT [0x25] = make_IDT_entry (&_ISR_25, code_descriptor);
	IDT [0x26] = make_IDT_entry (&_ISR_26, code_descriptor);
	IDT [0x27] = make_IDT_entry (&_ISR_27, code_descriptor);
	IDT [0x28] = make_IDT_entry (&_ISR_28, code_descriptor);
	IDT [0x29] = make_IDT_entry (&_ISR_29, code_descriptor);
	IDT [0x2A] = make_IDT_entry (&_ISR_2A, code_descriptor);
	IDT [0x2B] = make_IDT_entry (&_ISR_2B, code_descriptor);
	IDT [0x2C] = make_IDT_entry (&_ISR_2C, code_descriptor);
	IDT [0x2D] = make_IDT_entry (&_ISR_2D, code_descriptor);
	IDT [0x2E] = make_IDT_entry (&_ISR_2E, code_descriptor);
	IDT [0x2F] = make_IDT_entry (&_ISR_2F, code_descriptor);

	install_IDT (IDT, 0x30);
}
