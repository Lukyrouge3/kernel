

compile-boot: BUILD_DIR
	nasm -f bin boot/boot.asm -o build/boot.bin

BUILD_DIR:
	mkdir -p build