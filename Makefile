

compile_boot: BUILD_DIR
	nasm -f bin boot/boot.asm -o build/boot.bin

build_disk: compile_boot
	dd if=/dev/zero of=build/floppy.img bs=512 count=2880
	dd if=build/boot.bin of=build/floppy.img conv=notrunc bs=512 count=1

run_bootloader: build_disk
	qemu-system-i386 -drive format=raw,file=build/floppy.img,index=0,if=floppy

BUILD_DIR:
	mkdir -p build