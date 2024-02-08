CC=i686-elf-gcc
LD=i686-elf-ld
NASM=nasm
KERNEL_OFFSET=0x1000

SRCS=$(wildcard src/kernel/*.c src/driver/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

CFLAGS=-ffreestanding -Wall -Wextra -pedantic -Werror
INCLUDES=-I src/kernel -I src/driver
LDFLAGS=--oformat binary -Ttext

TARGET=drewos-image

.PHONY: all clean
all: $(TARGET)
clean:
	$(RM) *.o *.bin $(OBJS) *.dis $(TARGET) $(DEPS)

# Build object files from C sources.
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c -o $@ $<

# Kernel entrypoint compiled as elf.
kernel_entry.o: src/boot/kernel_entry.asm
	$(NASM) -I src/boot -f elf $^ -o $@

# Bootloader.
bootloader.bin: src/boot/bootloader.asm
	$(NASM) -I src/boot -f bin -o $@ $^

# Note: kernel_entry.o MUST be the first input file passed to the linker.
kernel.bin: kernel_entry.o $(OBJS)
	$(LD) $(LDFLAGS) $(KERNEL_OFFSET) -o $@ $^

# The disk image.
$(TARGET): bootloader.bin kernel.bin
	cat $^ >$@

-include $(DEPS)
