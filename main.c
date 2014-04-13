#include "pic.h"
#include "floppy.h"
#include "dma.h"

void _keyboard_int(int keycode) {
    *(char*)(0xb8004) = keycode;
}

void print_number(uint16_t x, int offset) {
    char buf[4];
    char ch;
    int c;

    for (c = 3; c >= 0; c--) {
        ch = x & 0xf;
        if (ch < 10) {
            ch = ch + '0';
        } else {
            ch = ch + 'A' - 10;
        }
        buf[c] = ch;
        x >>= 4;
    }

    for (c = 0; c < 4; c++) {
        *(char*)(0xb8000 + c*2 + offset) = buf[c];
    }
}

void clear_screen() {
    int i;
    for (i = 0; i < 2000; i++) {
        *(char*)(0xb8000 + i * 2) = 0x20;
    }
}

__attribute__((naked)) void _main() {
    clear_screen();

    pic_remap(0x20, 0x28);
    pic_mask(0xbd, 0xff);
    pic_enable();

    floppy_init();

    uint8_t buffer[512];

    floppy_read(buffer, 0, 0, 1);

    buffer[0] = 0xab;

    floppy_write(buffer, 0, 0, 1);
    floppy_read(buffer, 0, 0, 1);

    print_number(buffer[0], 0x10);
    print_number(buffer[1], 0x18);
    print_number(buffer[2], 0x20);
    print_number(buffer[3], 0x28);
    print_number(buffer[4], 0x30);

    *(int*)(0xb8000) = 0x203a2028;
    while (1) {}
}
