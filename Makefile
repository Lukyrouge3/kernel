TARGET=i686-elf
CC=$(TARGET)-gcc
LD=$(TARGET)-ld

# Source directories
SRC_DIR=kernel/src
ASM_DIR=kernel/asm
INC_DIR=kernel/includes
BUILD_DIR=build

# Find all C source files recursively
SRCS=$(shell find $(SRC_DIR) -name '*.c')
# Convert source paths to object paths in build directory
OBJS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS)) \
	 $(patsubst $(ASM_DIR)/%.asm,$(BUILD_DIR)/%.o,$(shell find $(ASM_DIR) -name '*.asm'))

CFLAGS=-std=c11 -O2 -Wall -Wextra \
       -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
       -m32 -I $(INC_DIR)
LDFLAGS=-T kernel/linker.ld -nostdlib

.PHONY: all clean run compile_boot build_disk lint

all: run

# Linting with cppcheck (static analysis for C)
lint:
	@echo "Running cppcheck..."
	cppcheck --enable=all --inconclusive --std=c11 \
		-I $(INC_DIR) \
		--suppress=missingIncludeSystem \
		--suppress=unusedFunction:kernel/src/main.c \
		$(SRCS)

compile_boot: $(BUILD_DIR)
	nasm -f bin boot/boot.asm -o $(BUILD_DIR)/boot.bin

build_disk: compile_boot kernel.bin
	dd if=/dev/zero of=$(BUILD_DIR)/floppy.img bs=512 count=2880
	dd if=$(BUILD_DIR)/boot.bin of=$(BUILD_DIR)/floppy.img conv=notrunc bs=512 count=1
	dd if=$(BUILD_DIR)/kernel.bin of=$(BUILD_DIR)/floppy.img bs=512 seek=1 conv=notrunc
	@echo "Kernel size: $$(stat -c%s $(BUILD_DIR)/kernel.bin) bytes"
	@echo "Sectors needed: $$(( ($$(stat -c%s $(BUILD_DIR)/kernel.bin) + 511) / 512 ))"

run: build_disk
	clear
	qemu-system-i386 -fda $(BUILD_DIR)/floppy.img -boot a -serial stdio

# Pattern rule for compiling C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(ASM_DIR)/%.asm | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

kernel.elf: $(OBJS) kernel/linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $(BUILD_DIR)/$@

kernel.bin: kernel.elf
	i686-elf-objcopy -O binary $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)