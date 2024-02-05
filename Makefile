CC=i686-elf-gcc
LD=i686-elf-ld
NASM=nasm
KERNEL_OFFSET=0x1000

CFLAGS=-ffreestanding -Wall -Wextra -pedantic -Werror
LDFLAGS=--oformat binary -Ttext

.PHONY: all clean
all: drewos-image
clean:
	$(RM) *.bin *.o *.dis drewos-image

# Compilation of the kernel.
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $^ -o $@

# Further C source files may be added here.
vga.o: vga.c
	$(CC) $(CFLAGS) -c $^ -o $@

# Kernel entrypoint compiled as elf.
kernel_entry.o: kernel_entry.asm
	$(NASM) -f elf $^ -o $@

# Bootloader.
bootloader.bin: bootloader.asm
	$(NASM) -f bin -o $@ $^

# Note: kernel_entry.o MUST be the first input file passed to the linker.
kernel.bin: kernel_entry.o kernel.o vga.o
	$(LD) $(LDFLAGS) $(KERNEL_OFFSET) -o $@ $^

# The disk image.
drewos-image: bootloader.bin kernel.bin
	cat $^ >$@
