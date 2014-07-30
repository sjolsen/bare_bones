#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __attribute__ ((packed)) __attribute__ ((aligned (8))) {
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
                         uint8_t privilege, bool page_granularity);

GDT_entry make_data_GDT (uint32_t base, uint32_t limit,
                         bool writable, bool downward,
                         uint8_t privilege, bool page_granularity);

void install_GDT (const GDT_entry* base, uint32_t entries);
void reload_segments (uint32_t code_descriptor, uint32_t data_descriptor);

#endif
