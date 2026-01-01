TARGET=i686-elf
CC=$(TARGET)-gcc
LD=$(TARGET)-ld

CFLAGS=-std=c11 -O2 -Wall -Wextra \
       -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
       -m32
LDFLAGS=-T kernel/linker.ld -nostdlib

compile_boot: BUILD_DIR
	nasm -f bin boot/boot.asm -o build/boot.bin

build_disk: compile_boot kernel.bin
	dd if=/dev/zero of=build/floppy.img bs=512 count=2880
	dd if=build/boot.bin of=build/floppy.img conv=notrunc bs=512 count=1
	dd if=build/kernel.bin of=build/floppy.img bs=512 seek=1 conv=notrunc

run_bootloader: build_disk
	qemu-system-i386 -drive format=raw,file=build/floppy.img,index=0,if=floppy

main.o: kernel/main.c
	$(CC) $(CFLAGS) -c $< -o build/$@

kernel.elf: main.o kernel/linker.ld
	$(LD) $(LDFLAGS) build/main.o -o build/$@

kernel.bin: kernel.elf
	i686-elf-objcopy -O binary build/kernel.elf build/kernel.bin

BUILD_DIR:
	mkdir -p build