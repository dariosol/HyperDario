CROSS = aarch64-linux-gnu-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS = -Wall -O2 -ffreestanding -nostdlib -Iinclude

# Source files
C_SRC = src/main.c src/uart.c src/trap.c src/guest.c
S_SRC = src/start.S src/vec_el2.S src/el2_to_el1.S 

# Object files
OBJ = $(C_SRC:.c=.o) $(S_SRC:.S=.o)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

all: hypervisor.bin

hypervisor.elf: $(OBJ)
	$(LD) -T linker.ld -o hypervisor.elf $(OBJ)

hypervisor.bin: hypervisor.elf
	$(OBJCOPY) -O binary hypervisor.elf hypervisor.bin

run: hypervisor.elf
	qemu-system-aarch64 -machine virt,virtualization=on -cpu cortex-a53 -kernel hypervisor.elf -nographic -serial mon:stdio

clean:
	rm -f src/*.o hypervisor.elf hypervisor.bin
