CROSS = aarch64-linux-gnu-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only -Iinclude

SRC = $(wildcard src/*.c)
ASM = $(wildcard src/*.S)
OBJ = $(SRC:.c=.o) $(ASM:.S=.o)

all: hypervisor.bin

hypervisor.bin: $(OBJ)
	$(LD) -T linker.ld -o hypervisor.elf $(OBJ)
	$(OBJCOPY) -O binary hypervisor.elf hypervisor.bin

run: hypervisor.bin
	qemu-system-aarch64 -machine virt,virtualization=on -cpu cortex-a53 -kernel hypervisor.elf -nographic -serial mon:stdio
	

clean:
	rm -f src/*.o hypervisor.elf hypervisor.bin
