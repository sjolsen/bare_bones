#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "ddump.h"

/* #define INTERRUPT(name, body) __attribute__ ((naked)) void name (void) \
{ \
	__asm__ ("pushal"); \
	{ body } \
	__asm__ ("popal"); \
	__asm__ ("iret"); \
}

INTERRUPT (ISR_alert, {
	vga_putline ("Received an interrupt");
})



typedef struct __attribute__ ((packed)) {
	uint8_t gate_type : 4;
	uint8_t segment   : 1;
	uint8_t privilege : 2;
	uint8_t present   : 1;
} typeattr;

typedef struct __attribute__ ((packed)) {
	uint16_t entry_low;
	uint16_t CS_selector;
	uint8_t  zero;
	typeattr type;
	uint16_t entry_high;
} IDT_entry;

static inline
IDT_entry make_basic_IDT_entry (void (*address) (void))
{
	return (IDT_entry) {
		(uintptr_t) address,
		0,
			0,
			};
} */

void kernel_main (multiboot_info_t* info, uint32_t magic)
{
	vga_initialize ();

	GDT_entry GDT [] = {
		make_data_GDT (0, 0, false, false, 0, 0),
		make_code_GDT (0, -1, true, false, 0, 1),
		make_data_GDT (0, -1, true, false, 0, 1)
	};
	install_GDT (GDT, 3);
	reload_segments (1, 2);
	vga_putline ("GDT Loaded");

	vga_putline ("System halt");
}
