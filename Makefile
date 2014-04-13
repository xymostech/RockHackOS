.PHONY: run clean debug

run: disk.flp
	qemu-system-i386 -fda $<

%.bin: %.asm
	nasm -f bin -o $@ $<

%.o: %.c $(wildcard *.h)
	clang -O1 -c -o $@ $< -ffreestanding -nostdlib -nostdinc -m32 -std=c99

%.o: %.asm
	nasm -f elf32 -o $@ $<

os.bin: os.o main.o pic.o io.o floppy.o dma.o
	ld -o $@ $^ --oformat binary -mi386linux -Ttext -0x20

disk.flp: boot.bin os.bin
	cat $^ > disk.flp

debug: disk.flp
	qemu-system-i386 -fda $< -s -S

clean:
	rm -f *.o *.bin *.flp
