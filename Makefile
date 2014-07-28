AS = as
ASFLAGS = -march=i686 -m32
CC = gcc
CFLAGS = -march=i686 -m32 -std=c99 -ffreestanding -Wall -Wextra
LD = ld
LDFLAGS = -march=i686 -melf_i386 -nostdlib

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

multiboot.bin: multiboot.o main.o
	$(LD) $(LDFLAGS) -T multiboot.ld -o $@ $^

multiboot.o: multiboot.s
	$(AS) $(ASFLAGS) -o $@ $^

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f multiboot.o main.o
