AS = as
ASFLAGS = -march=i686 --32
CC = gcc
CFLAGS = -march=i686 -m32 -std=c99 -ffreestanding -Os -Wall -Wextra -Werror
LD = ld
LDFLAGS = -march=i686 -melf_i386 -nostdlib

CSOURCES := $(wildcard *.c)
COBJECTS := $(patsubst %.c,%.c.o,$(CSOURCES))
CDEPENDS := $(patsubst %.c,%.c.d,$(CSOURCES))

ASMSOURCES := $(wildcard *.s)
ASMOBJECTS := $(patsubst %.s,%.s.o,$(ASMSOURCES))

DEPENDS := $(CDEPENDS)
OBJECTS := $(COBJECTS) $(ASMOBJECTS)

.PHONY: all test clean cleandeps

all: kernel.bin
test: kernel.bin
	qemu-system-i386 -kernel $<

clean:
	rm -f kernel.bin $(OBJECTS)
cleandeps:
	rm -f $(DEPENDS)

kernel.bin: kernel.ld $(DEPENDS) $(OBJECTS)
	$(LD) $(LDFLAGS) -T $< -o $@ $(OBJECTS) --gc-sections #--print-gc-sections

-include $(DEPENDS)

$(OBJECTS): Makefile

%.c.d: %.c
	$(CC) -MM $< | sed 's/^\(.*\)\.o:/\1.c.d \1.c.o:/' > $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.s.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<
