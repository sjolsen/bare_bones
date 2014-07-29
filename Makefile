AS = as
ASFLAGS = -march=i686 --32
CC = gcc
CFLAGS = -march=i686 -m32 -std=c99 -ffreestanding -Wall -Wextra
LD = ld
LDFLAGS = -march=i686 -melf_i386 -nostdlib

CSOURCES := $(wildcard *.c)
COBJECTS := $(patsubst %.c,%.c.o,$(CSOURCES))
CDEPENDS := $(patsubst %.c,%.c.d,$(CSOURCES))

ASMSOURCES := $(wildcard *.s)
ASMOBJECTS := $(patsubst %.s,%.s.o,$(ASMSOURCES))

DEPENDS := $(CDEPENDS)
OBJECTS := $(COBJECTS) $(ASMOBJECTS)

.PHONY: all
all: kernel.iso

kernel.iso: multiboot.bin
	$(eval ISODIR := $(shell mktemp -d))
	mkdir -p $(ISODIR)
	mkdir -p $(ISODIR)/boot
	cp $< $(ISODIR)/boot/
	mkdir -p $(ISODIR)/boot/grub
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISODIR)
	rm -rf $(ISODIR)

multiboot.bin: $(DEPENDS) $(OBJECTS)
	$(LD) $(LDFLAGS) -T multiboot.ld -o $@ $(OBJECTS)

%.c.d: %.c
	$(CC) -o $@ -MM $<

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.s.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

-include $(DEPENDS)

.PHONY: clean
clean:
	rm -f kernel.iso multiboot.bin $(OBJECTS)

.PHONY: test
test: kernel.iso
	qemu-system-i386 -cdrom $<
