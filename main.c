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

	print_multiboot_memmap (info);
}



typedef struct vbeptr {
	uint16_t offset;
	uint16_t segment;
} vbeptr;

static inline
void* decode_vbeptr (vbeptr ptr)
{
	return (void*) (((uint32_t) ptr.segment << 4) + ptr.offset);
}

typedef struct __attribute__ ((packed)) VbeInfoBlock {
	uint8_t  VbeSignature [4];
	uint16_t VbeVersion;
	vbeptr   OemStringPtr;
	uint8_t  Capabilities [4];
	vbeptr   VideoModePtr;
	uint16_t TotalMemory;
	uint16_t OemSoftwareRev;
	vbeptr   OemVendorNamePtr;
	vbeptr   OemProductNamePtr;
	vbeptr   OemProductRevPtr;
	uint8_t  Reserved [222];
	uint8_t  OemData [256];
} VbeInfoBlock;

void print_vbe_mode (uint16_t mode)
{
	char buffer [5];
	vga_puts ("0x");
	vga_puts (format_uint (buffer, mode, 4, 16));
}

void print_vbe_info (const multiboot_info_t* info)
{
	if (!(info->flags & MULTIBOOT_INFO_VBE_INFO))
		return;

	const VbeInfoBlock* vbe_ctlinfo = (const VbeInfoBlock*) info->vbe_control_info;
	vga_putline ("VBE info:");
	vga_puts ("  Version: ");
	char buffer [4] = "0.0";
	buffer [0] = '0' + (vbe_ctlinfo->VbeVersion >> 8);
	buffer [2] = '0' + (vbe_ctlinfo->VbeVersion & 0xFF);
	vga_putline (buffer);

	vga_puts ("  OEM:     ");
	vga_putline (decode_vbeptr (vbe_ctlinfo->OemStringPtr));
	vga_puts ("  Vendor:  ");
	vga_putline (decode_vbeptr (vbe_ctlinfo->OemVendorNamePtr));
	vga_puts ("  Product: ");
	vga_puts (decode_vbeptr (vbe_ctlinfo->OemProductNamePtr));
	vga_puts (" ");
	vga_putline (decode_vbeptr (vbe_ctlinfo->OemProductRevPtr));

	vga_putline ("Video modes:");
	uint8_t counter = 0;
	for (const uint16_t* mode = decode_vbeptr (vbe_ctlinfo->VideoModePtr); *mode != 0xFFFF; ++mode) {
		if (counter == 0)
			vga_puts ("  ");
		else
			vga_puts (" ");
		print_vbe_mode (*mode);
		if (++counter == 10) {
			counter = 0;
			vga_putline ("");
		}
	}
	if (counter != 0)
		vga_putline ("");
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
	print_vbe_info (info);
}
