#include "multiboot.h"
#include "vga.h"
#include "format.h"
#include "portio.h"
#include "ddump.h"
#include <stdbool.h>

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

typedef struct __attribute__ ((packed)) {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t  base_high1;
	uint8_t  accessed    : 1;
	uint8_t  rw_bit      : 1;
	uint8_t  dc_bit      : 1;
	uint8_t  ex_bit      : 1;
	uint8_t  one         : 1; // Unused
	uint8_t  privilege   : 2;
	uint8_t  present     : 1;
	uint8_t  limit_high  : 4;
	uint8_t  zero        : 2; // Unused
	uint8_t  mode32      : 1;
	uint8_t  granularity : 1;
	uint8_t  base_high2;
} GDT_entry;

_Static_assert (sizeof (GDT_entry) == 8, "GDT not packed");

GDT_entry make_code_GDT (uint32_t base, uint32_t limit,
                         bool readable, bool conforming,
                         uint8_t privilege, bool page_granularity)
{
	return (GDT_entry) {
		.limit_low   = limit & 0xFFFF,
		.base_low    = base & 0xFFFF,
		.base_high1  = (base >> 16) & 0xFF,
		.rw_bit      = readable,
		.dc_bit      = conforming,
		.ex_bit      = 1,
		.one         = 1,
		.privilege   = privilege,
		.present     = 1,
		.limit_high  = (limit >> 16) & 0xFF,
		.zero        = 0,
		.mode32      = 1,
		.granularity = page_granularity,
		.base_high2  = (base >> 24) & 0xFF
	};
}

GDT_entry make_data_GDT (uint32_t base, uint32_t limit,
                         bool writable, bool downward,
                         uint8_t privilege, bool page_granularity)
{
	return (GDT_entry) {
		.limit_low   = limit & 0xFFFF,
		.base_low    = base & 0xFFFF,
		.base_high1  = (base >> 16) & 0xFF,
		.rw_bit      = writable,
		.dc_bit      = downward,
		.ex_bit      = 0,
		.one         = 1,
		.privilege   = privilege,
		.present     = 1,
		.limit_high  = (limit >> 16) & 0xFF,
		.zero        = 0,
		.mode32      = 1,
		.granularity = page_granularity,
		.base_high2  = (base >> 24) & 0xFF
	};
}

void install_GDT (const GDT_entry* base, uint32_t entries)
{
	struct {
		uint16_t length;
		uint32_t base;
	} GDT = {entries / sizeof (GDT_entry), (uintptr_t) base};
	__asm__ ("lgdt (%0)" :: "p" (&GDT));
}

void reload_segments (uint32_t code_descriptor, uint32_t data_descriptor)
{
	code_descriptor *= sizeof (GDT_entry);
	data_descriptor *= sizeof (GDT_entry);
	__asm__ (
		"mov %0, %%cs\n"
		"mov %1, %%ds\n"
		"mov %1, %%es\n"
		"mov %1, %%fs\n"
		"mov %1, %%gs\n"
		"mov %1, %%ss\n"
		:: "r" (code_descriptor), "r" (data_descriptor)
	);
}

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

	data_dump ((uintptr_t) GDT, GDT, GDT + 3);

	vga_putline ("System halt");
}
