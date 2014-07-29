#include "multiboot.h"
#include "vga.h"
#include "format.h"
#include "portio.h"
#include "ddump.h"

static
void format_mmap (const multiboot_memory_map_t* mmap)
{
	char buffer [9];
	vga_puts ("0x");
	vga_puts (format_uint (buffer, mmap->addr >> 32, 8, 16));
	vga_puts (format_uint (buffer, mmap->addr, 8, 16));
	vga_puts ("-0x");
	vga_puts (format_uint (buffer, (mmap->addr + mmap->len - 1) >> 32, 8, 16));
	vga_puts (format_uint (buffer, (mmap->addr + mmap->len - 1), 8, 16));

	static const char* description [] = {
		[MULTIBOOT_MEMORY_AVAILABLE]        = " AVAILABLE",
		[MULTIBOOT_MEMORY_RESERVED]         = " RESREVED",
		[MULTIBOOT_MEMORY_ACPI_RECLAIMABLE] = " ACPI_RECLAIMABLE",
		[MULTIBOOT_MEMORY_NVS]              = " NVS",
		[MULTIBOOT_MEMORY_BADRAM]           = " BADRAM"
	};
	vga_putline (description [mmap->type]);
}

static inline
const multiboot_memory_map_t* first_memory_map_t (uint32_t addr)
{
	return (const multiboot_memory_map_t*) ((uintptr_t) addr);
}

static inline
const multiboot_memory_map_t* next_memory_map_t (const multiboot_memory_map_t* mmap)
{
	return (const multiboot_memory_map_t*) ((uintptr_t) mmap + mmap->size + 4);
}

void kernel_main (multiboot_info_t* info, uint32_t magic)
{
	vga_initialize ();

	for (const multiboot_memory_map_t* mmap = first_memory_map_t (info->mmap_addr);
	     mmap < first_memory_map_t (info->mmap_addr + info->mmap_length);
	     mmap = next_memory_map_t (mmap)) {
		vga_color oldcolor = vga_getcolor ();
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
			vga_setcolor (make_vga_color (COLOR_WHITE, oldcolor.bg));
		format_mmap (mmap);
		vga_setcolor (oldcolor);
	}

	vga_putline ("System halt");
}
