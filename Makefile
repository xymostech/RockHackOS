.PHONY: run

run: disk.flp
	qemu-system-i386 -fda $<

%.bin: %.asm
	nasm -f bin -o $@ $<

%.o: %.c
	clang -c -o $@ $< -ffreestanding -nostdlib -nostdinc -m32

%.o: %.asm
	nasm -f elf32 -o $@ $<

os.bin: os.o main.o
	ld -o $@ $^ --oformat binary -mi386linux -Ttext -0x20

disk.flp: boot.bin os.bin
	cat $^ > disk.flp

debug: disk.flp
	qemu-system-i386 -fda $< -s -S
