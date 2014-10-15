#include "multiboot.h"
#include "vga.h"
#include "GDT.h"
#include "IDT.h"
#include "ISR.h"
#include "IRQ.h"
#include "format.h"
#include "ddump.h"

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (*a))

void print_multiboot_magic (uint32_t magic)
{
	vga_puts ("Magic:\n  ");
	vga_color oldcolor = vga_getcolor ();
	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
		vga_setcolor (make_vga_color (COLOR_GREEN, oldcolor.bg));
	else
		vga_setcolor (make_vga_color (COLOR_RED, oldcolor.bg));
	vga_puts ("0x");
	char buffer [9];
	vga_putline (format_uint (buffer, magic, 8, 16));
	vga_setcolor (oldcolor);
}

void print_multiboot_flags (const multiboot_info_t* info)
{
	static const char* flagnames [] = {
		"MULTIBOOT_INFO_MEMORY",
		"MULTIBOOT_INFO_BOOTDEV",
		"MULTIBOOT_INFO_CMDLINE",
		"MULTIBOOT_INFO_MODS",
		"MULTIBOOT_INFO_AOUT_SYMS",
		"MULTIBOOT_INFO_ELF_SHDR",
		"MULTIBOOT_INFO_MEM_MAP",
		"MULTIBOOT_INFO_DRIVE_INFO",
		"MULTIBOOT_INFO_CONFIG_TABLE",
		"MULTIBOOT_INFO_BOOT_LOADER_NAME",
		"MULTIBOOT_INFO_APM_TABLE",
		"MULTIBOOT_INFO_VBE_INFO",
		"MULTIBOOT_INFO_FRAMEBUFFER_INFO"
	};

	vga_putline ("Flags:");
	size_t index = 0;
	uint32_t mask = 1;
	while (index < ARRAY_LENGTH (flagnames)) {
		if (info->flags & mask) {
			vga_puts ("  ");
			vga_putline (flagnames [index]);
		}
		++index;
		mask <<= 1;
	}
}

const multiboot_memory_map_t* mmap_begin (const multiboot_info_t* info)
{
	return (const multiboot_memory_map_t*) info->mmap_addr;
}

const multiboot_memory_map_t* mmap_end (const multiboot_info_t* info)
{
	return (const multiboot_memory_map_t*) (info->mmap_addr + info->mmap_length);
}

const multiboot_memory_map_t* mmap_next (const multiboot_memory_map_t* map)
{
	uint32_t size = map->size + sizeof (map->size);
	const char* map_addr = (const char*) map;
	const char* next_addr = map_addr + size;
	return (const multiboot_memory_map_t*) next_addr;
}

void print_multiboot_memmap_entry (const multiboot_memory_map_t* map)
{
	static const char* typenames [] = {
		NULL,
		"AVAILABLE       ",
		"RESERVED        ",
		"ACPI_RECLAIMABLE",
		"NVS             ",
		"BADRAM          "
	};

	char buffer [17];
	vga_puts ("  ");
	vga_puts (typenames [map->type]);
	vga_puts (" [0x");
	vga_puts (format_uint (buffer, map->addr, 16, 16));
	vga_puts (" - 0x");
	vga_puts (format_uint (buffer, map->addr + map->len - 1, 16, 16));
	vga_putline ("]");
}

void print_multiboot_memmap (const multiboot_info_t* info)
{
	if (!(info->flags & MULTIBOOT_INFO_MEM_MAP))
		return;

	vga_putline ("Memory map:");
	uint64_t memsize = 0;
	uint64_t amemsize = 0;
	for (const multiboot_memory_map_t* map = mmap_begin (info);
	     map != mmap_end (info);
	     map = mmap_next (map)) {
		print_multiboot_memmap_entry (map);
		memsize += map->len;
		if (map->type == MULTIBOOT_MEMORY_AVAILABLE)
			amemsize += map->len;
	}

	char buffer [21];
	vga_puts ("  Total ");
	vga_puts (format_uint (buffer, memsize, 0, 10));
	vga_puts (" bytes (");
	vga_puts (format_uint (buffer, amemsize, 0, 10));
	vga_putline (" bytes available)");
}

void print_multiboot_info (const multiboot_info_t* info, uint32_t magic)
{
	print_multiboot_magic (magic);
	if (info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME)
		vga_putline ((const char*) info->boot_loader_name);

	print_multiboot_flags (info);
	print_multiboot_memmap (info);
}

void kernel_main (const multiboot_info_t* info, uint32_t magic)
{
	GDT_initialize ();
	IDT_initialize ();
	ISR_table_initialize (&null_ISR);
	vga_initialize ();
	ISR_table [INT_divide_by_zero] = &debug_ISR;
	IRQ_disable (IRQ_PIT);

	__asm__ ("sti"::);
	print_multiboot_info (info, magic);
}
