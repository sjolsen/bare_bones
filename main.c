#include "multiboot.h"
#include "vga.h"
#include "format.h"
#include "portio.h"
#include "ddump.h"

void kernel_main (multiboot_info_t* info, uint32_t magic)
{
	vga_initialize ();

	char buffer [33];
	vga_puts ("Multiboot info (magic 0x");
	vga_puts (format_uint32_t (buffer, magic, 8, 16));
	vga_putline ("):");
	data_dump ((uintptr_t) info, info, info + 1);

	vga_puts ("MULTIBOOT_INFO_MEMORY:           "); vga_putchar (info->flags & MULTIBOOT_INFO_MEMORY           ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_BOOTDEV:          "); vga_putchar (info->flags & MULTIBOOT_INFO_BOOTDEV          ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_CMDLINE:          "); vga_putchar (info->flags & MULTIBOOT_INFO_CMDLINE          ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_MODS:             "); vga_putchar (info->flags & MULTIBOOT_INFO_MODS             ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_AOUT_SYMS:        "); vga_putchar (info->flags & MULTIBOOT_INFO_AOUT_SYMS        ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_ELF_SHDR:         "); vga_putchar (info->flags & MULTIBOOT_INFO_ELF_SHDR         ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_MEM_MAP:          "); vga_putchar (info->flags & MULTIBOOT_INFO_MEM_MAP          ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_DRIVE_INFO:       "); vga_putchar (info->flags & MULTIBOOT_INFO_DRIVE_INFO       ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_CONFIG_TABLE:     "); vga_putchar (info->flags & MULTIBOOT_INFO_CONFIG_TABLE     ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_BOOT_LOADER_NAME: "); vga_putchar (info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_APM_TABLE:        "); vga_putchar (info->flags & MULTIBOOT_INFO_APM_TABLE        ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_VBE_INFO:         "); vga_putchar (info->flags & MULTIBOOT_INFO_VBE_INFO         ? '1' : '0'); vga_putchar ('\n');
	vga_puts ("MULTIBOOT_INFO_FRAMEBUFFER_INFO: "); vga_putchar (info->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO ? '1' : '0'); vga_putchar ('\n');

	if (info->flags & MULTIBOOT_INFO_CMDLINE) {
		vga_puts ("Command line: ");
		vga_putline ((const char*) info->cmdline);
	}

	/* vga_puts ("Multiboot flags: 0b"); */
	/* vga_putline (format_uint32_t (buffer, info->flags, 32, 2)); */

	/* vga_putline ("Multiboot header:"); */
	/* struct multiboot_header* header = (struct multiboot_header*) (1 << 20); */
	/* data_dump ((uintptr_t) header, header, header + 1); */

	vga_putline ("System halt");
}
