.PHONY: run clean debug

run: disk.flp
	cp $< $<.real
	qemu-system-i386 -fda $<.real

continue: disk.flp
	qemu-system-i386 -fda $<.real

%.bin: %.asm
	nasm -f bin -o $@ $<

%.fs: %.asm
	nasm -f bin -o $@ $^

%.o: %.c $(wildcard *.h)
	clang -O1 -c -o $@ $< -ffreestanding -nostdlib -nostdinc -m32 -std=c99

%.o: %.asm
	nasm -f elf32 -o $@ $<

os.bin: os.o main.o pic.o io.o floppy.o dma.o keyboard.o hexes.o fs.o shell.o mem.o
	ld -o $@ $^ --oformat binary -mi386linux -Ttext -0x20

disk.flp: boot.bin os.bin testfs.fs
	rm -f disk.flp
	dd if=boot.bin of=$@ bs=512
	dd conv=notrunc if=os.bin of=$@ bs=512 seek=1
	dd conv=notrunc if=testfs.fs of=$@ bs=512 seek=36

debug: disk.flp
	qemu-system-i386 -fda $< -s -S

clean:
	rm -f *.o *.bin *.flp
