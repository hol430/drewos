CC=i686-elf-gcc
LD=i686-elf-ld
NASM=nasm
KERNEL_OFFSET=0x1000

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

CFLAGS=-ffreestanding -Wall -Wextra -pedantic -Werror
LDFLAGS=--oformat binary -Ttext

TARGET=drewos-image

.PHONY: all clean
all: $(TARGET)
clean:
	$(RM) *.bin $(OBJS) *.dis $(TARGET)

# Build object files from C sources.
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

# Kernel entrypoint compiled as elf.
kernel_entry.o: kernel_entry.asm
	$(NASM) -f elf $^ -o $@

# Bootloader.
bootloader.bin: bootloader.asm
	$(NASM) -f bin -o $@ $^

# Note: kernel_entry.o MUST be the first input file passed to the linker.
kernel.bin: kernel_entry.o $(OBJS)
	$(LD) $(LDFLAGS) $(KERNEL_OFFSET) -o $@ $^

# The disk image.
$(TARGET): bootloader.bin kernel.bin
	cat $^ >$@

-include $(DEPS)
