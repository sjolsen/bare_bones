OSNAME = "RDPOS"

AS = as
override ASFLAGS:=$(ASFLAGS) -march=i686 --32
CC = gcc
override CFLAGS:=$(CFLAGS) -march=i686 -m32 -std=c99 -ffreestanding -Os -Wall -Wextra -Werror
LD = ld
override LDFLAGS:=$(LDFLAGS) -march=i686 -melf_i386 -nostdlib

NOROMFLAG = -netdev user,id=hostnet0 -device virtio-net-pci,romfile=,netdev=hostnet0 # Kill iPXE option ROM
override QEMUFLAGS:=$(NOROMFLAG) $(QEMUFLAGS)

CSOURCES := $(wildcard *.c)
COBJECTS := $(patsubst %.c,%.c.o,$(CSOURCES))
CDEPENDS := $(patsubst %.c,%.c.d,$(CSOURCES))

ASMSOURCES := $(wildcard *.s)
ASMOBJECTS := $(patsubst %.s,%.s.o,$(ASMSOURCES))

DEPENDS := $(CDEPENDS)
OBJECTS := $(COBJECTS) $(ASMOBJECTS)

.PHONY: all depends test test-grub clean cleandeps

all: kernel.bin
depends: $(DEPENDS)

test: kernel.bin
	qemu-system-i386 -kernel $< $(QEMUFLAGS)
test-grub: kernel.iso
	qemu-system-i386 -cdrom $< $(QEMUFLAGS)

clean:
	rm -f kernel.iso kernel.bin $(OBJECTS)
cleandeps:
	rm -f $(DEPENDS)

kernel.bin: kernel.ld $(OBJECTS)
	$(LD) $(LDFLAGS) -T $< -o $@ $(OBJECTS) --gc-sections #--print-gc-sections

kernel.iso: kernel.bin
	$(eval ISODIR := $(shell mktemp -d))
	mkdir -p $(ISODIR)
	mkdir -p $(ISODIR)/boot
	cp $< $(ISODIR)/boot/
	mkdir -p $(ISODIR)/boot/grub
	echo 'menuentry "'"$(OSNAME)"'" {\n multiboot /boot/'"$<"'\n}' > $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISODIR)
	rm -rf $(ISODIR)

-include $(DEPENDS)

$(OBJECTS): Makefile

%.c.d: %.c
	$(CC) -MM $< | sed 's/^\(.*\)\.o:/\1.c.d \1.c.o:/' > $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.s.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<
